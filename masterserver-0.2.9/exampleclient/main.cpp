#include <config.h>

#include <unistd.h>
#include <iostream>

#include "HeartbeatThread.hpp"
#include "InfoThread.hpp"

int main(int , char** )
{
    HeartbeatThread* heartbeatThread = 0;
    InfoThread* infoThread = 0;
    
    try {
        infoThread = new InfoThread(1234);
        heartbeatThread = new HeartbeatThread("localhost", 28900,
                "example", 1234);

        // here comes game logic...
        sleep(20 * 60);

        delete infoThread;
        delete heartbeatThread;
    } catch(std::exception& e) {
        delete heartbeatThread;
        std::cerr << "Error: " << e.what() << std::endl;
    }    
    
    return 0;
}
