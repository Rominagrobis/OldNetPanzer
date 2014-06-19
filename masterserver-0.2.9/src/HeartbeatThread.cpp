/*
Copyright (C) 2004 Matthias Braun <matze@braunis.de>
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>

#include "HeartbeatThread.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include <time.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Log.hpp"
#include "Config.hpp"
#include "SocketStream.hpp"
#include "MasterServer.hpp"
#include "Tokenizer.hpp"

namespace masterserver
{

HeartbeatThread::HeartbeatThread(MasterServer* newmasterserver)
    : masterserver(newmasterserver), running(false)
{
    // try to get list of masterservers
    *log << "Querying neighbor masterserver." << std::endl;
    requestMasterServerList();
   
    // start thread
    if(pthread_create(&thread, 0, threadMain, this) != 0) {
        throw std::runtime_error("Couldn't create Heartbeat Thread");
    }
}

HeartbeatThread::~HeartbeatThread()
{
    // signal thread to stop
    running = false;
    pthread_join(thread, 0);

    writeNeighborCache();
    std::stringstream packet;
    packet << "\\quit\\gamename\\master\\password\\"
        << config->getSection("server").getValue("masterserver-password")
        << "\\port\\" << config->getSection("server").getValue("port")
        << "\\final\\";
    sendPacket(packet.str());
}

void
HeartbeatThread::readNeighborCache()
{
    iniparser::Store neighborini;

    const std::string& neighborcachefile =
        config->getSection("server").getValue("neighborcachefile");
    std::ifstream in(neighborcachefile.c_str());
    if(!in.good()) {
        *log << "Couldn't open neighborcache file '" << neighborcachefile
            << "'." << std::endl;
        return;
    }
    neighborini.load(in);

    iniparser::Section& neighbors = neighborini.getSection("neighbors");
    for(int i=0;  ; ++i) {
        std::stringstream keynamestr;
        keynamestr << "ip" << i;
        
        std::string keyname = keynamestr.str();
        if(!neighbors.exists(keyname))
            break;
        
        const std::string& ip = neighbors.getValue(keyname);

        int port = 28900;
        std::stringstream portnamestr;
        portnamestr << "port" << i;
        std::string portname = portnamestr.str();
        if(neighbors.exists(portname)) {
            port = neighbors.getIntValue(portname);
        }

        in_addr_t in_addr = inet_addr(ip.c_str());
        if(in_addr == 0) {
            *log << "Invalid entry in neighbor file: " << ip << std::endl;
            continue;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = in_addr;
        addr.sin_port = htons(port);
    
        masterserver->addServer("master", addr);
    }
}

void HeartbeatThread::requestMasterServerList()
{
    // parse config
    readNeighborCache();

    // query addresses for masterserverlist
    std::vector<const ServerInfo*> list;
    masterserver->getServerList(list, "master");
    for(std::vector<const ServerInfo*>::iterator i = list.begin();
            i != list.end(); ++i) {
        const ServerInfo* info = *i;
        try {
            requestMasterServerList2(info->address);
            break;
        } catch(std::exception& e) {
            *log << "Failed contacting neighbor " 
                << inet_ntoa(info->address.sin_addr)
                << ":" << ntohs(info->address.sin_port) << std::endl;
        }
    }

    masterserver->getServerList(list, "master");
    if(list.size() == 0) {
        *log << "No additional master servers found." << std::endl;
    }
}

void HeartbeatThread::requestMasterServerList2(struct sockaddr_in addr)
{
    int sock = -1;

    try {
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock < 0) {
            std::stringstream msg;
            msg << "Couldn't create socket: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }
        
        // connect to server
        int res = connect(sock, (struct sockaddr*) &addr,
                sizeof(addr));
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't connect to masterserver: "
                << strerror(errno) << "\n";
            throw std::runtime_error(msg.str());
        }

        SocketStream stream(sock);
        sock = -1; // stream has control over the socket now
        
        stream << "\\list\\gamename\\master\\final\\" << std::flush;
        Tokenizer* tokenizer = new Tokenizer(stream);
        std::string ip = "";
        int port = 28900;
        while(!stream.eof()) {
            std::string token = tokenizer->getNextToken();
            if(token == "ip") {
                if(ip != "")
                    addMasterServer(ip, port);

                ip = tokenizer->getNextToken();
                port = 28900;
            } else if(token == "port") {
                std::stringstream portstr(tokenizer->getNextToken());
                portstr >> port;
            } else if(token == "final") {
                break;
            } else {
                *log << "Unknown token when querying for masterserverlist: " <<
                    token << std::endl;
            }
        }
        if(ip != "")
            addMasterServer(ip, port);

        delete tokenizer;
    } catch(std::exception& e) {
        if(sock>=0)
            close(sock);
        *log << "Couldn't request a list of other masterservers: " << e.what()
            << std::endl;
    } catch(...) {
        if(sock>=0)
            close(sock);
        throw;
    }
}

void
HeartbeatThread::addMasterServer(const std::string& address, int port)
{
    // lookup server address
    struct hostent* hentry = gethostbyname(address.c_str());
    if(!hentry) {
        *log << "Couldn't lookup address of masterserver '"
            << address << "'" << std::endl;
        return;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr*) hentry->h_addr)->s_addr;
    serveraddr.sin_port = htons(port);

    if (!masterserver->addServer("master", serveraddr)) {
        *log << "Not adding additional masterserver '" << address
            << "': already in list." << std::endl;
        return;                                                         
    }                                                                   

    *log << "Found additional masterserver '" << address << "'" << std::endl;
}

void* HeartbeatThread::threadMain(void* data)
{
    HeartbeatThread* _this = reinterpret_cast<HeartbeatThread*> (data);
   
    _this->running = true;
    while(_this->running) {
        std::stringstream packet;
        packet << "\\heartbeat\\port\\" <<
            config->getSection("server").getValue("port")
               << "\\gamename\\master\\password\\" << 
               config->getSection("server").getValue("masterserver-password")
               << "\\final\\";
        _this->sendPacket(packet.str());

        // use nanosleep to have a thread cancelation point
        time_t startsleep = time(0);
        int sleeptime = config->getSection("server").
            getIntValue("masterserver-heartbeat-interval");
        while(_this->running) {
            timespec ntime = { 0, 300000 };
            nanosleep(&ntime, 0);
            if(time(0) - startsleep > sleeptime)
                break;
        }

        _this->writeNeighborCache();
    }

    return 0;
}

void
HeartbeatThread::sendPacket(const std::string& packet)
{
    std::vector<const ServerInfo*> list;
    masterserver->getServerList(list, "master");   
    for(std::vector<const ServerInfo*>::iterator i = list.begin();
            i != list.end(); ++i) {
        const ServerInfo* info = *i;
        try {
            sendPacket(info->address, packet);
        } catch(std::exception& e) {
            *log << "Couldn't send heartbeat packet to '"
                << inet_ntoa(info->address.sin_addr) 
                << "': " << e.what() << std::endl;
        } catch(...) {
            *log << "Unexpected exception while sending heartbeat."<< std::endl;
        }
    }
}

void
HeartbeatThread::sendPacket(struct sockaddr_in serveraddr,
        const std::string& packet)
{
    int sock = -1;
    int res;
    try {
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock < 0) {
            std::stringstream msg;
            msg << "Couldn't create socket: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }

        // connect to server
        res = connect(sock, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't connect to masterserver: "
                << strerror(errno) << "\n";
            throw std::runtime_error(msg.str());
        }

        // send heartbeat packet
        SocketStream stream(sock);
        sock = -1; // stream has control of socket now

        stream << packet << std::flush;
    } catch(...) {
        if(sock>=0)
            close(sock);
        throw;
    }
}

void
HeartbeatThread::writeNeighborCache()
{
    if(config->getSection("server").getBoolValue("write-back-neighbour-list") == false)
        return;
    
    iniparser::Store neighborini;
    iniparser::Section& neighbors = neighborini.getSection("neighbors");

    std::vector<const ServerInfo*> serverlist;
    masterserver->getServerList(serverlist, "master");

    int idx = 0;
    for(std::vector<const ServerInfo*>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        const ServerInfo* info = *i;
        std::stringstream key;
        key << "ip" << idx;
        neighbors.setValue(key.str(), inet_ntoa(info->address.sin_addr));
        std::stringstream port;
        port << "port" << idx;
        neighbors.setIntValue(port.str(), ntohs(info->address.sin_port));
        idx++;
    }

    const std::string& neighborcachefile 
        = config->getSection("server").getValue("neighborcachefile");
    std::ofstream out(neighborcachefile.c_str());
    neighborini.save(out);
    if(!out.good()) {
        *log << "Couldn't write neighborcachefile '" << neighborcachefile
            << "'." << std::endl;
        return;
    }
}

} // end of namespace masterserver
