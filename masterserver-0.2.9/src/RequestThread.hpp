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
#ifndef __REQUESTTHREAD_HPP__
#define __REQUESTTHREAD_HPP__

#include <iostream>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace masterserver
{

class MasterServer;
class SocketStream;

/** A thread that handles a single client request */
class RequestThread
{
public:
    RequestThread(MasterServer* master, SocketStream* stream,
            struct sockaddr_in addr);
    ~RequestThread();

    time_t getStartTime()
    { return starttime; }
    
private:
    static void* threadMain(void* data);
    void run();
    
    MasterServer* masterserver;
    SocketStream* stream;
    struct sockaddr_in addr;
    pthread_t thread;
    time_t starttime;
    pthread_mutex_t exit_sync;
    volatile bool running;
};

}

#endif

