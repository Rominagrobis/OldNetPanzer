bin_PROGRAMS = masterserver
masterserver_SOURCES = Config.hpp Config.cpp Log.cpp Log.hpp main.cpp   \
                MasterServer.cpp MasterServer.hpp                       \
                RequestThread.cpp RequestThread.hpp ServerInfo.hpp      \
                SocketStream.hpp Tokenizer.hpp HeartbeatThread.hpp      \
                HeartbeatThread.cpp                                     \
                iniparser/Section.cpp iniparser/Section.hpp             \
                iniparser/Store.cpp iniparser/Store.hpp
masterserver_CXXFLAGS = -DLOCALSTATEDIR=\"${localstatedir}\" -DSYSCONFDIR=\"${sysconfdir}\"
masterserver_LDFLAGS = -pthread
