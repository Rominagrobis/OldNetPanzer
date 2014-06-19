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

#include "MasterServer.hpp"

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Tokenizer.hpp"
#include "SocketStream.hpp"
#include "Log.hpp"
#include "Config.hpp"
#include "iniparser/Section.hpp"

namespace masterserver
{

MasterServer::MasterServer()
    : serverconfig(config->getSection("server")), running(false)
{   
    sock = -1;
    try {
        // bind to socket
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock < 0) {
            std::stringstream msg;
            msg << "Couldn't create socket: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }

        // we want to start at once and not waiting for sockets to get into
        // clean state again (as this waiting was > 1 hour in 1 case)
        int val = 1;
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
            std::cerr << "Warning: Couldn't set SO_REUSEADDR: "
                << strerror(errno) << std::endl;
        }

        // resolved bind/listen address
        struct hostent* hentry;
        hentry =
            gethostbyname(serverconfig.getValue("listen").c_str());
        if(!hentry) {
            std::stringstream msg;
            msg << "Couldn't resolve listen-address '" << 
                serverconfig.getValue("listen") << "'.";
            throw std::runtime_error(msg.str());
        }
        int bindaddress = ((struct in_addr*) hentry->h_addr)->s_addr;
        if(bindaddress == 0 && serverconfig.getValue("ip") == "") {
            throw std::runtime_error(
                    "Must specify an explicit bind address or an IP in config");
        }
       
        memset(&serveraddr, 0, sizeof(struct sockaddr_in));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = bindaddress;
        serveraddr.sin_port = htons(serverconfig.getIntValue("port"));
        int res = bind(sock, (struct sockaddr*) &serveraddr,
                sizeof(struct sockaddr_in));
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't bind socket to port " <<
                serverconfig.getIntValue("port") << " : " << strerror(errno);
            throw std::runtime_error(msg.str());
        }

        // eventually resolve fake address
        if(serverconfig.getValue("ip") != "") {
            hentry = gethostbyname(serverconfig.getValue("ip").c_str());
            if(!hentry) {
                std::stringstream msg;
                msg << "Couldn't resolve ip-address '"
                    << serverconfig.getValue("ip") << "'.";
                throw std::runtime_error(msg.str());
            }
            serveraddr.sin_addr.s_addr = 
                ((struct in_addr*) hentry->h_addr)->s_addr;
        }

        res = listen(sock, 511);
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't listen on socket: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }
        
        pthread_mutex_init(&serverlist_mutex, 0);
    } catch(...) {
        if(sock >= 0)
            close(sock);
        throw;
    }
}

MasterServer::~MasterServer()
{
    for(std::list<RequestThread*>::iterator i = threads.begin();
            i != threads.end(); ++i) {
        delete *i;
    }

    if(sock>=0)
        close(sock);
}

void
MasterServer::cancel()
{
    running = false;
}

void
MasterServer::run()
{   
    running = true;
    
    while(1) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sock, &set);

        int res = 0;
        do {
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
           
            fd_set testset = set;
            res = select(FD_SETSIZE, &testset, 0, 0, &timeout);
            if(res < 0 || !running)
                return;
        } while(res == 0);

        // accept the client
        struct sockaddr_in clientaddr;
        socklen_t socklen = sizeof(struct sockaddr_in);
        int clientsock = accept(sock, (struct sockaddr*) &clientaddr,
                &socklen);
        if(clientsock < 0) {
            *log << "Accept error: " << strerror(errno) << std::endl;
            continue;
        }

        // remove too old threads
        std::list<RequestThread*>::iterator i;
        time_t currenttime = time(0);
        for(i = threads.begin(); i != threads.end(); ) {
            if(currenttime - (*i)->getStartTime() >
                    serverconfig.getIntValue("client-request-timeout")) {
                delete *i;
                i = threads.erase(i);
            } else {
                ++i;
            }               
        }
        if(threads.size() > (size_t) serverconfig.getIntValue("connection-limit")) {
            // drop the client, we're too busy
            close(clientsock);
            continue;
        }

        SocketStream* stream = new SocketStream(clientsock);
        RequestThread* thread = 0;
        try {
            thread = new RequestThread(this, stream, clientaddr);
        } catch(std::exception* e) {
            *log << "error starting requestthread! This is very bad"
                 << std::endl;
            delete thread;
            delete stream;
            continue;
        }
        threads.push_back(thread);
    }
}

void
MasterServer::parseKeyValues(std::map<std::string, std::string>& pairs,
        std::iostream& stream, Tokenizer& tokenizer)
{
    while(!stream.eof()) {
        std::string key = tokenizer.getNextToken();
        if(key == "final")
            break;
        
        std::string value = tokenizer.getNextToken();
        if(value == "final") {
            *log << "malformed key/value pair: " << key << "/" << value
                 << std::endl;
            break;
        }
        pairs.insert(std::make_pair(key, value));
    }
}

void
MasterServer::parseHeartbeat(std::iostream& stream,
        struct sockaddr_in* addr, Tokenizer& tokenizer)
{
    std::map<std::string, std::string> keyvalues;
    parseKeyValues(keyvalues, stream, tokenizer);

    std::map<std::string, std::string>::iterator v
        = keyvalues.find("gamename");
    if(v == keyvalues.end()) {
        *log << "gamename missing for heartbeat." << std::endl;
        stream << "\\error\\no gamename specified\\final\\" << std::flush;
        return;
    }
    const std::string& gamename = v->second;

    v = keyvalues.find("port");
    if(v == keyvalues.end()) {
        *log << "port missing for heartbeat." << std::endl;
        stream << "\\error\\no port specified\\final\\" << std::flush;
        return;
    }
    std::stringstream strstream(v->second);
    int queryport;
    strstream >> queryport;

    // security check for master/master update
    if(gamename == "master") {
        v = keyvalues.find("password");
        if(v == keyvalues.end()) {
            *log << "password missing for master heartbeat." << std::endl;
            stream << "\\error\\passwort missing for master update\\final\\"
                << std::flush;
            return;
        }
        const std::string& password = v->second;
        if(password != serverconfig.getValue("masterserver-password")) {
            *log << "wrong password for master heartbeat." << std::endl;
            stream << "\\error\\wrong password for master update\\final\\"
                   << std::flush;
            return;
        }     
    }

    checkTimeouts();

    pthread_mutex_lock(&serverlist_mutex);
    
    ServerInfo* info = 0;
    for(std::vector<ServerInfo>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        if(i->address.sin_addr.s_addr == addr->sin_addr.s_addr
            && i->address.sin_port == htons(queryport)) {
            *log << "Heartbeat from " << inet_ntoa(addr->sin_addr) 
                 << ":" << queryport << " (" << gamename << ")" << std::endl;
            info = &(*i);
        }
    }
    if(info == 0) {
        *log << "New server at " << inet_ntoa(addr->sin_addr) 
             << ":" << queryport << " (" << gamename << ")" << std::endl;
        serverlist.push_back(ServerInfo());
        info = & (serverlist.back());
    }

    // update ServerInfo
    info->address = *addr;
    info->address.sin_port = htons(queryport);
    info->settings = keyvalues;
    info->lastheartbeat = time(0);
    
    pthread_mutex_unlock(&serverlist_mutex);

    stream << "\\final\\" << std::flush;
}

bool
MasterServer::addServer(const std::string& gamename, struct sockaddr_in addr)
{
    // don't add yourself
    if(addr.sin_addr.s_addr == serveraddr.sin_addr.s_addr
        && addr.sin_port == serveraddr.sin_port)
        return false;

    pthread_mutex_lock(&serverlist_mutex);
    std::vector<ServerInfo>::iterator i;
    for(i = serverlist.begin(); i != serverlist.end(); ++i) {
        if(i->address.sin_addr.s_addr == addr.sin_addr.s_addr
            && i->address.sin_port == addr.sin_port) {
            // already there...
            i->settings.clear();
            i->settings.insert(
                    std::make_pair(std::string("gamename"), gamename));
            i->lastheartbeat = time(0);
            pthread_mutex_unlock(&serverlist_mutex);
            return false;
        }
    }

    ServerInfo info;
    info.settings.insert(
            std::make_pair(std::string("gamename"), gamename));
    info.address = addr;
    info.lastheartbeat = time(0);    
    
    serverlist.push_back(info);
    pthread_mutex_unlock(&serverlist_mutex);

    return true;
}

void
MasterServer::parseList(std::iostream& stream, struct sockaddr_in* addr,
        Tokenizer& tokenizer, bool gameSpyHacks)
{
    time_t currenttime = time(0);

    std::map<std::string, std::string> keyvalues;
    parseKeyValues(keyvalues, stream, tokenizer);

    // get the name of the game
    std::map<std::string, std::string>::iterator v
        = keyvalues.find("gamename");
    if(v == keyvalues.end()) {
        *log << "Missing gamename in list query." << std::endl;
        stream << "\\error\\missing gamename in list query\\final\\" <<
            std::flush;
        return;
    }
    const std::string& gamename = v->second;
    *log << "List query for game '" << gamename << "' from "
        << inet_ntoa(addr->sin_addr) << std::endl;

    checkTimeouts();
            
    pthread_mutex_lock(&serverlist_mutex);
    std::map<std::string, std::string>::iterator v2;
    for(std::vector<ServerInfo>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        // match against requested key/values
        bool match = true;
        for(v = keyvalues.begin(); v != keyvalues.end(); ++v) {
            // find matching key/value pair
            v2 = i->settings.find(v->first);
            if(v2 == i->settings.end()) { // no matching key found
                match = false;
                break;
            }
            if(v2->second != v->second) { // value differs
                match = false;
                break;
            }
        }
        if(!match)
            continue;
        
        if(!gameSpyHacks) {
            stream << "\\ip\\" << inet_ntoa(i->address.sin_addr)
                << "\\port\\" << ntohs(i->address.sin_port);
        } else {
            // qstat/gamespy hack
            stream << "\\ip\\" << inet_ntoa(i->address.sin_addr)
                << ":" << ntohs(i->address.sin_port);
        }
    }
    pthread_mutex_unlock(&serverlist_mutex);

    // we're a masterserver ourself (TODO handle additional match criteria)
    if(gamename == "master") {
        stream << "\\ip\\" << inet_ntoa(serveraddr.sin_addr)
            << "\\port\\" << ntohs(serveraddr.sin_port);
    }
    
    stream << "\\final\\" << std::flush;
}

void
MasterServer::parseQuit(std::iostream& stream, struct sockaddr_in* addr,
        Tokenizer& tokenizer)
{
    std::map<std::string, std::string> keyvalues;
    parseKeyValues(keyvalues, stream, tokenizer);

    int port = 0;
    std::map<std::string, std::string>::iterator v
        = keyvalues.find("port");
    if(v == keyvalues.end()) {
        *log << "Missing port in Quit message from " 
            << inet_ntoa(addr->sin_addr) << std::endl;
        stream << "\\error\\missing port in quit message\\final\\"
            << std::flush;
        return;
    }
    std::stringstream portstr(v->second);
    portstr >> port;
        
    checkTimeouts();

    pthread_mutex_lock(&serverlist_mutex);
    bool found = false;
    for(std::vector<ServerInfo>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        if(i->address.sin_addr.s_addr != addr->sin_addr.s_addr
            || i->address.sin_port != htons(port))
            continue;

        *log << "Quit from server at " << inet_ntoa(i->address.sin_addr) 
            << ":" << ntohs(i->address.sin_port) << " ("
            << i->settings["gamename"] << ")" << std::endl;
        serverlist.erase(i);
        found = true;
        break;
    }
    pthread_mutex_unlock(&serverlist_mutex);
    
    if(!found) {
        *log << "No server known for quit request (from " << 
            inet_ntoa(addr->sin_addr) << " server at " << port
            << ")" << std::endl;
    }
    stream << "\\final\\" << std::flush;
}

void
MasterServer::checkTimeouts()
{
    time_t currenttime = time(0);
    
    pthread_mutex_lock(&serverlist_mutex);
    for(std::vector<ServerInfo>::iterator i = serverlist.begin();        
            i != serverlist.end(); /* nothing */) {
        ServerInfo& info = *i;
        if(currenttime - info.lastheartbeat >=
                serverconfig.getIntValue("server-alive-timeout")) {
            // remove server from list
            *log << "server timeout at " 
                << inet_ntoa(info.address.sin_addr)
                << ":" << ntohs(info.address.sin_port) 
                << " (" << info.settings["gamename"] << ")" << std::endl;
            i = serverlist.erase(i);
            continue;
        }

        ++i;
    }
    pthread_mutex_unlock(&serverlist_mutex);
}

void
MasterServer::getServerList(std::vector<const ServerInfo*>& list,
        const std::string& gamename)
{
    checkTimeouts();

    list.clear();
    pthread_mutex_lock(&serverlist_mutex);
    for(std::vector<ServerInfo>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        const ServerInfo& info = *i;
        std::map<std::string, std::string>::const_iterator i 
            = info.settings.find("gamename");
        if(i == info.settings.end())
            continue;
        if(i->second != gamename)
            continue;

        list.push_back(&info);
    }
    pthread_mutex_unlock(&serverlist_mutex);
}

}
