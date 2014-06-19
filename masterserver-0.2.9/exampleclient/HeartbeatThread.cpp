#include <config.h>

#include "HeartbeatThread.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <time.h>
#include <errno.h>
#include <unistd.h> 
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// send a heartbeat packet every 5 minutes
static const int UPDATEINTERVAL = 5*60;

HeartbeatThread::HeartbeatThread(const std::string& masteraddress,
        int masterport, const std::string& newgamename, int gamestatusport)
    : running(false), gamename(newgamename), gameport(gamestatusport)
{
    // lookup server address
    struct hostent* hentry;                                                     
    hentry = gethostbyname(masteraddress.c_str());
    if(!hentry) {                                                               
        std::stringstream msg;
        msg << "Couldn't resolve address of masterserver '"
            << masteraddress << "'";
        throw std::runtime_error(msg.str());
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr*) hentry->h_addr)->s_addr;
    serveraddr.sin_port = htons(masterport);

    // send initial heartbeat
    sendHeartbeat();

    // start thread
    pthread_create(&thread, 0, threadMain, this);
}

HeartbeatThread::~HeartbeatThread()
{
    // signal thread to stop
    running = false;
    pthread_cancel(thread);
    pthread_join(thread, 0);
}

void* HeartbeatThread::threadMain(void* data)
{
    HeartbeatThread* _this = reinterpret_cast<HeartbeatThread*> (data);
    
    _this->running = true;
    while(_this->running) {
        // use nanosleep to have a thread cancelation point
        timespec sleeptime = { UPDATEINTERVAL, 0 };
        nanosleep(&sleeptime, 0);
        
        try {
            _this->sendHeartbeat();
        } catch(std::exception& e) {
            std::cerr << "Couldn't send heartbeat packet: " << e.what() << "\n"
                << "retrying in " << UPDATEINTERVAL << "seconds." << std::endl;
        } catch(...) {
            std::cerr << "Unexpected exception while sending heartbeat.\n";
        }
    }

    return 0;
}

void HeartbeatThread::sendHeartbeat()
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
        res = connect(sock, (struct sockaddr*) &serveraddr,
                sizeof(struct sockaddr_in));
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't connect to masterserver: "
                << strerror(errno) << "\n";
            throw std::runtime_error(msg.str());
        }

        // send heartbeat packet
        std::stringstream packet;
        packet << "\\heartbeat\\" << gameport
               << "\\gamename\\" << gamename << "\\final\\";
        const void* data = packet.str().c_str();
        size_t datasize = packet.str().size();
        res = send(sock, data, datasize, 0);
        if(res != (int) datasize) {
            std::stringstream msg;
            msg << "Couldn't send heartbeat packet: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }

        close(sock);
    } catch(...) {
        if(sock>=0)
            close(sock);
        throw;
    }
}
