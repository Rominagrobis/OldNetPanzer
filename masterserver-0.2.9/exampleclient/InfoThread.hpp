#ifndef __INFOTHREAD_HPP__
#define __INFOTHREAD_HPP__

#include <sstream>
#include <pthread.h>

/** This class will create an additonal info thread and is responsible for
 * answering queries about the server status.
 */
class InfoThread
{
public:
    InfoThread(int port);
    ~InfoThread(); 

private:
    static void* threadMain(void* data);
    void handleStatusRequests();

    void sendInfo(std::stringstream& out);
    void sendRules(std::stringstream& out);
    void sendPlayers(std::stringstream& out);
    
    int sockfd;
    bool running;
    pthread_t thread;
};

#endif
