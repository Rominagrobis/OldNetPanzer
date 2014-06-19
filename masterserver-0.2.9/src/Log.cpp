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

#include "Log.hpp"

#include <time.h>
#include <ios>
#include <iostream>
#include <fstream>

#include "Config.hpp"

namespace masterserver
{

/** A streambuf implementation that wraps around an existing streambuf and
 * outputs the current timestampe after each newline
 */
class FileStampStreamBuf : public std::streambuf
{
public:
    FileStampStreamBuf(std::ostream& newstream)
        : needstamp(true)
    {
        stream = &newstream;
        setp(buf, buf+sizeof(buf));
    }

    virtual int overflow(int c)
    {
        if(pbase() == pptr())
            return 0;
       
        if(needstamp)
            outputdate();
        needstamp = false;

        // check for \n in the buffer
        char* chunkstart = pbase();
        for(char* p = pbase(); p != pptr(); ++p) {
            if(*p == '\n') {
                // output data in buffer so far
                stream->write(chunkstart, p-chunkstart+1);
                if(p < pptr()-1)
                    outputdate();
                else
                    needstamp = true;
                chunkstart = p+1;
            }
        }
        // output the rest
        stream->write(chunkstart, pptr() - chunkstart);

        if(c != traits_type::eof()) {
            *stream << (char) c;
            if(c == '\n') {
                needstamp = true;
            }
        }
        setp(buf, buf+sizeof(buf));
        return 0;
    }

    virtual int sync()
    {
        overflow(traits_type::eof());
        stream->flush();
        return 0;
    }

private:
    void outputdate()
    {
        char timestamp[64];
        time_t curtime = time(0);
        struct tm* loctime = localtime(&curtime);
        strftime(timestamp, sizeof(timestamp), "<%F %T>", loctime);
        *stream << timestamp;
    }

    std::ostream* stream;

    bool needstamp;
    char buf[1024];
};

class OFileStampStream : public std::ostream
{
public:
    OFileStampStream(std::ostream& stream)
        : std::ostream(new FileStampStreamBuf(stream))
    { }

    ~OFileStampStream()
    {
        delete rdbuf();
    }
};

std::ostream* log = &std::cout;

std::ostream* logfile = 0;

void initializeLog()
{
    // try to open a file
    const std::string& logfilename
        = config->getSection("server").getValue("logfile");
    
    logfile = new std::ofstream(logfilename.c_str(), std::ios_base::app);
    if(!logfile->good()) {
        std::cout << "Couldn't open logfile '" << logfilename
                  << "' falling back to stdout.\n";
        delete logfile;

        log = new OFileStampStream(std::cout);
    } else {
        log = new OFileStampStream(*logfile);
        *log << "\nRestarted masterserver" << std::endl;
    }
}

void closeLog()
{
    if(logfile) {
        *log << "Logfile closed." << std::endl;
        delete logfile;
    }
    if(log != &std::cout) {
        delete log;
        log = &std::cout;
    }
}

}
