#include <config.h>

#include "InfoThread.hpp"

#include <sstream>
#include <stdexcept>

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Tokenizer.hpp"

InfoThread::InfoThread(int port)
{
    // setup socket and bind it
    sockfd = -1;
    try {
        sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sockfd < 0) {
            std::stringstream msg;
            msg << "Couldn't create socket: " << strerror(errno);
            throw std::runtime_error(msg.str());
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        int res = bind(sockfd, (struct sockaddr*) &addr, sizeof(addr));
        if(res < 0) {
            std::stringstream msg;
            msg << "Couldn't bind socket to port " << port << ": " 
                << strerror(errno);
            throw std::runtime_error(msg.str());
        }
        
        // start the thread
        pthread_create(&thread, 0, threadMain, this);
    } catch(...) {
        if(sockfd >= 0)
            close(sockfd);
        throw;
    }
}

InfoThread::~InfoThread()
{
    running = false;
    pthread_cancel(thread);
    pthread_join(thread, 0);
    
    if(sockfd >= 0)
        close(sockfd);
}

void* InfoThread::threadMain(void* data)
{
    InfoThread* _this = reinterpret_cast<InfoThread*> (data);

    _this->running = true;
    while(_this->running) {
        try {
            _this->handleStatusRequests();
        } catch(std::exception& e) {
            std::cerr << "Error while handling status request: " << e.what() <<
                "\n";
        } catch(...) {
            std::cerr << "Unexpected exception in InfoThread.\n";
        }
    }

    return 0;
}

void InfoThread::handleStatusRequests()
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buf[4096];
    int res 
        = recvfrom(sockfd, buf, sizeof(buf), 0,
                (struct sockaddr*) &addr, &addrlen);
    if(res <= 0) {
        std::stringstream msg;
        msg << "Couldn't receive network data: " << strerror(errno);
        throw std::runtime_error(msg.str());
    }
    std::cout << "Received stuff\n";

    std::string packet(buf, res);
    Tokenizer tokenizer(packet);

    std::string query;
    do {
        query = tokenizer.getNextToken();
        std::cout << "Query: " << query << "\n";

        if(query == "status") {
            std::stringstream response; 
            // you should make sure the response packet doesn't get > 1500bytes
            sendInfo(response);
            sendRules(response);
            sendPlayers(response);
            response << "\\final\\";

            std::cout << "Resulting size: " << response.str().length() << "\n";
            const void* data = response.str().c_str();
            size_t datasize = response.str().size();
            res = sendto(sockfd, data, datasize, 0,
                    (struct sockaddr*) &addr, sizeof(addr));
            if(res < 0) {
                std::stringstream msg;
                msg << "Errro when sending back info: " << strerror(errno);
                throw std::runtime_error(msg.str());
            }

            break; // enough, next client
        } else {
            // unknown query skip it
        }
    } while(query != "");
}

void InfoThread::sendInfo(std::stringstream& out)
{
    // This should be some game-specific logic...
    out << "gamename\\example\\"
        << "gamever\\16\\"
        << "hostname\\Example Client\\"
        << "mapname\\testing ground\\"
        << "gametype\\default\\" // no special mod running
        << "numplayers\\5\\"
        << "maxplayers\\8\\"
        << "gamemode\\timelimit\\";
}

void InfoThread::sendRules(std::stringstream& out)
{
    // send some gamespecific settings
    out << "friendlyfire\\1\\"
        << "timelimit\\20\\"
        << "fraglimit\\300\\";
}

void InfoThread::sendPlayers(std::stringstream& out)
{
    // send more gamespecific stuff about player 0
    out << "player_0\\Anonymias\\"
        << "kills_0\\12\\"
        << "deaths_0\\13\\"
        << "points_0\\123\\"
        << "ping\\75\\"
        << "team_0\\alliance1\\";

    // player 1
    out << "player_0\\Another\\"
        << "kills_0\\12\\"
        << "deaths_0\\13\\"
        << "points_0\\123\\"
        << "ping\\75\\"
        << "team_0\\alliance1\\";

    // Info about the "alliance1" team
    out << "team_t0\\alliance1\\"
        << "score_t0\\12\\";
}

