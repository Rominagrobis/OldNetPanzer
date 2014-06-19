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

#include "Config.hpp"
#include "Log.hpp"
#include <fstream>

static const char* CONFIGFILE = SYSCONFDIR"/masterserver.cfg";
static const char* LOGFILE = LOCALSTATEDIR"/log/masterserver.log";
static const char* SERVERCACHE = LOCALSTATEDIR"/cache/masterserver/neighbors";
static const char* PIDFILE = LOCALSTATEDIR"/run/masterserver.pid";

namespace masterserver
{

iniparser::Store* config = 0;

void loadConfig()
{
    config = new iniparser::Store();

    /* Config defaults */
    iniparser::Section& server = config->getSection("server");
    server.setValue("logfile", LOGFILE);
    server.setValue("neighborcachefile", SERVERCACHE);
    server.setValue("pidfile", PIDFILE);
    server.setValue("listen", "0.0.0.0");
    server.setValue("ip", "");
    server.setValue("port", "28900");
    server.setValue("server-alive-timeout", "720");
    server.setValue("client-request-timeout", "30");
    server.setValue("connection-limit", "10");
    server.setValue("masterserver-heartbeat-interval", "300");
    server.setValue("masterserver-password", "");
    server.setValue("masterserver-neighboors", "");
    server.setValue("write-back-neighbour-list", "no");

    std::ifstream in(CONFIGFILE);
    if(!in.good()) {
        *log << "Couldn't load configfile '" << CONFIGFILE << "'.\n";    
        return;                                                          
    }
    config->load(in);        
}

void freeConfig()
{
    delete config;
    config = 0;
}

} // end of namespace masterserver
