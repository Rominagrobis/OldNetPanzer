/*
Copyright (C) 2012 by Aaron Perez <aaronps@gmail.com>

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

#ifndef HTTPCLIENTSOCKET_HPP
#define HTTPCLIENTSOCKET_HPP

#include "Network/TCPSocket.hpp"
#include <vector>
#include <string>
#include <map>
#include <stdint.h>

class HTTPClientSocket : public network::TCPSocketObserver
{
public:
    HTTPClientSocket();
    virtual ~HTTPClientSocket();

    void setMethod(const std::string& method);
    void setHeader(const std::string& name, const std::string& value);
    void setContent(const std::vector<uint8_t> request_content);
    void setHost(const std::string& host);
    void setPort(const std::string& port);
    void setPath(const std::string& path);
    void doRequest();

protected:
    void onDataReceived(network::TCPSocket *so, const char *data, const int len);
    void onConnected(network::TCPSocket *so);
    void onDisconected(network::TCPSocket *so);
    void onSocketError(network::TCPSocket *so);

    virtual void onContentFinished() {}

    std::string content;

private:
    network::TCPSocket * socket;
    std::string host;
    std::string port;
    std::string path;

    enum STATE
    {
        sERROR,
        sIDLE,
        sRESPONSE_PROTO,
        sRESPONSE_CODE,
        sRESPONSE_TEXT,
        sHEADER_ENDING,
        sHEADER_BEGIN,
        sHEADER_NAME,
        sHEADER_SKIP_SPACE,
        sHEADER_VALUE,
        sRECEIVING_DATA
    } state;

    std::string token;
    std::string header_name;

    std::string response_version;
    std::string response_code_str;
    std::string response_description;
    int response_code;
    int content_length;

    typedef std::map<std::string, std::string> headers;

    std::string request_method;
    headers request_headers;

    headers response_headers;

    std::vector<uint8_t> request_content;

    void _handleReceivedByte(uint8_t data);
    STATE _handleHeaders();
};

#endif // HTTPCLIENTSOCKET_HPP
