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

#include <iostream>
#include <exception>
#include <fstream>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "Log.hpp"
#include "Config.hpp"
#include "MasterServer.hpp"
#include "HeartbeatThread.hpp"

using namespace masterserver;

static MasterServer* masterserv = 0;
static HeartbeatThread* heartbeatthread = 0;

static void signalhandler(int signum)
{
    if(masterserv)
        masterserv->cancel();
}
    
int main(int , char** )
{   
    loadConfig();
    initializeLog();
    
    try {
        masterserv = new MasterServer;

        std::ofstream* out = 0;
        const std::string& pidfile = 
            config->getSection("server").getValue("pidfile");
        if(pidfile != "") {
            out = new std::ofstream(pidfile.c_str());
            if(!out->good()) {
               std::cerr << "Couldn't create pidfile '" << pidfile << "': ";
               perror(0);
               exit(EXIT_FAILURE);
            }
        }

        // fork
        pid_t pid = fork();
        if(pid < 0) {
            perror("Error while forking: ");
            exit(EXIT_FAILURE);
        } if(pid > 0) {
            exit(EXIT_SUCCESS);
        }

        pid_t sid = setsid();
        if(sid < 0) {
            *masterserver::log << "Couldn't get session ID: " << strerror(errno)
                << std::endl;
            exit(EXIT_FAILURE);
        }

        umask(0);

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        signal(SIGTERM, signalhandler);
        signal(SIGINT, signalhandler);
        signal(SIGQUIT, signalhandler);

        if(out) {
            *out << getpid() << std::endl;
            delete out;
        }

        heartbeatthread = new HeartbeatThread(masterserv);
        masterserv->run();
    } catch(std::exception& e) {
        *masterserver::log << "Fatal Error: " << e.what() << std::endl;
        std::cerr << "Fatal Error: " << e.what() << "\n";
        delete masterserv;
        masterserv = 0;

        closeLog();
        freeConfig();
        return 1;
    }

    delete heartbeatthread;
    heartbeatthread = 0;
    delete masterserv;
    masterserv = 0;

    const std::string& pidfile 
        = config->getSection("server").getValue("pidfile");
    if(pidfile != "") {
        unlink(pidfile.c_str());
    }
    
    closeLog();
    freeConfig();

    return 0;
}

