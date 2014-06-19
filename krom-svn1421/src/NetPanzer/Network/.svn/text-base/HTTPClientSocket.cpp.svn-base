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

#include "HTTPClientSocket.hpp"
#include "Util/Log.hpp"

#include <sstream>

HTTPClientSocket::HTTPClientSocket()
{
    request_method = "GET";
    port = "80";
}

HTTPClientSocket::~HTTPClientSocket()
{

}

void HTTPClientSocket::setMethod(const std::string &method)
{
    this->request_method.assign(method);
}

void HTTPClientSocket::setHeader(const std::string &name, const std::string &value)
{
    request_headers[name] = value;
}

void HTTPClientSocket::setContent(const std::vector<uint8_t> request_content)
{
    this->request_content.assign(request_content.begin(), request_content.end());

    std::stringstream ss;
    ss << request_content.size();
    request_headers["content-length"] = ss.str();
}

void HTTPClientSocket::setHost(const std::string &host)
{
    this->host.assign(host);
}

void HTTPClientSocket::setPort(const std::string &port)
{
    this->port.assign(port);
}

void HTTPClientSocket::setPath(const std::string &path)
{
    this->path.assign(path);
}

void HTTPClientSocket::doRequest()
{
    try
    {
        socket = new network::TCPSocket(host, port , this);
        state = sRESPONSE_PROTO;
    }
    catch (...)
    {
        if ( socket )
        {
            socket->destroy();
        }
        throw;
    }
}

void HTTPClientSocket::onConnected(network::TCPSocket *so)
{
    LOGGER.debug("HTTPClientSocket connected");
    std::stringstream ss;

    ss << request_method << " http://" << host << ':' << port << path << " HTTP/1.1\r\n"
       << "Host: " << host << ':' << port << "\r\n"
       << "Connection: close\r\n";

    for ( headers::iterator i = request_headers.begin(); i != request_headers.end(); i++ )
    {
        LOGGER.warning("adding header [%s]:[%s]", i->first.c_str(), i->second.c_str());
        ss << i->first << ": " << i->second << "\r\n";
    }

    ss << "\r\n";

    std::string t(ss.str());

    LOGGER.warning("Going to send:\n%s\n[END]", t.c_str());

    socket->send(t.c_str(), t.length());
}

void HTTPClientSocket::onDisconected(network::TCPSocket *so)
{
    LOGGER.debug("HTTPClientSocket disconnected");
    socket = 0;
}

void HTTPClientSocket::onSocketError(network::TCPSocket *so)
{
    LOGGER.debug("HTTPClientSocket error");
    socket = 0;
}

void HTTPClientSocket::onDataReceived(network::TCPSocket *so, const char *data, const int len)
{
    int mlen = len;
    do
    {
        _handleReceivedByte(*data);
        data++;
    } while ( --mlen );
}

void HTTPClientSocket::_handleReceivedByte(uint8_t data)
{
    switch (state)
    {
        case sERROR:
            break;
        case sIDLE:
            break;
        case sRESPONSE_PROTO:
            if ( ! isspace(data) )
            {
                token.append(1, tolower(data));
            }
            else
            {
                if ( token.compare("http/1.1") )
                {
                    LOGGER.warning("Received protocol is not 'HTTP/1.1'");
                }
                response_version = token;
                token.clear();
                content_length = 0;
                content.clear();
                state = sRESPONSE_CODE;
            }
            break;
        case sRESPONSE_CODE:
            if ( token.size() < 3 )
            {
                if ( isdigit(data) )
                {
                    token.append(1, data);
                }
                else
                {
                    LOGGER.warning("Wrong response code, is not a digit");
                    state = sERROR;
                }
            }
            else
            {
                if ( ! isspace(data) )
                {
                    LOGGER.warning("Wrong response code, doesn't end in space");
                }

                response_code_str = token;
                std::stringstream ss(token);
                ss >> response_code;

                token.clear();
                state = sRESPONSE_TEXT;
            }
            break;
        case sRESPONSE_TEXT:
            token.append(1, data); // actualy \r\n shouldn't be in the text
            if ( data == '\n' )
            {
                while ( !token.empty() && isspace(*token.rbegin()) )
                {
                    token.resize(token.size()-1);
                }
                response_description = token;
                state = sHEADER_BEGIN;
                token.clear();
            }
            break;
        case sHEADER_ENDING:
            if ( data == 10 )
            {
                token.clear();
                state = _handleHeaders();
            }
            else
            {
                state = sERROR;
            }
            break;
        case sHEADER_BEGIN:
            if ( data == 13 )
            {
                state = sHEADER_ENDING;
                break;
            }
            token.clear();
            state = sHEADER_NAME;
            // fall through, it is begin of name
        case sHEADER_NAME:
            if ( data == ':' )
            {
                header_name = token;
                token.clear();
                state = sHEADER_SKIP_SPACE;
            }
            else
            {
                token.append(1, tolower(data));
            }
            break;
        case sHEADER_SKIP_SPACE:
            if ( isspace(data) )
            {
                break;
            }
            state = sHEADER_VALUE;
            // fall through, it is not space

        case sHEADER_VALUE:
            if ( data == '\n' )
            {
                while ( !token.empty() && isspace(*token.rbegin()) )
                {
                    token.resize(token.size()-1);
                }
                response_headers[header_name] = token;
                // handle header here
                state = sHEADER_BEGIN;
            }
            else
            {
                token.append(1,data);
            }
            break;
        case sRECEIVING_DATA:
            content.append(1,data);
            if ( --content_length <= 0 )
            {
                onContentFinished();
                socket->destroy();
                state = sIDLE;
            }
            break;
    }
}

HTTPClientSocket::STATE HTTPClientSocket::_handleHeaders()
{

    LOGGER.warning("Received: [%s] [%s/%d] [%s]",
                   response_version.c_str(),
                   response_code_str.c_str(),
                   response_code,
                   response_description.c_str());

    for ( headers::iterator i = response_headers.begin(); i != response_headers.end(); i++ )
    {
        LOGGER.warning("received header [%s]:[%s]", i->first.c_str(), i->second.c_str());
    }


    headers::iterator clength = response_headers.find("content-length");
    if ( clength != response_headers.end() )
    {
        std::stringstream ss(clength->second);
        ss >> content_length;
    }

    if ( response_code < 100 )
    {
        return sERROR; // this is bad server
    }
    else if ( response_code < 200 )
    {
        return sRESPONSE_PROTO;
    }
    else if ( response_code < 300 )
    {
        if ( response_code == 204 ) // no content
            return sIDLE;

        if ( content_length > 0 )
            return sRECEIVING_DATA;

        return sIDLE;
    }
    else if ( response_code < 400 )
    {
        return sIDLE; // this is a redirect
    }
    else if ( response_code < 500 )
    {
        return sERROR; // this is client error
    }
    else if ( response_code < 600 )
    {
        return sERROR; // this is server error
    }
    else
    {
        return sERROR; // this is bad server
    }


}
