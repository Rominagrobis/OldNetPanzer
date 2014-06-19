/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Aaron Perez

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
 *
 * Created on September 19, 2012, 8:19 PM
 */

#ifndef CHATREQUEST_HPP
#define	CHATREQUEST_HPP

#include "Core/CoreTypes.hpp"
#include "PlayerRequests.hpp"
#include "Classes/Network/NetMessage.hpp"

#include <algorithm>

class ChatRequest : public NetMessage
{
public:
    enum { MAX_CHAT_LENGTH = _MAX_NET_PACKET_SIZE - 16 };
    
    char message_text[MAX_CHAT_LENGTH];

    ChatRequest()
    {
        message_class = _net_message_class_player_commands;
        message_id = PlayerRequests::CHAT;
    }

    unsigned int setText(const NPString& text)
    {
        unsigned int text_len = std::min((unsigned int)text.size(), (unsigned int)MAX_CHAT_LENGTH);
        text.copy(message_text, text_len);
        
        return text_len + sizeof(NetMessage);
    }
    
    void getText(int packet_length, NPString& text) const
    {
        unsigned int text_len = packet_length - sizeof(NetMessage);
        text.assign(message_text, text_len);
    }
} __attribute__((packed));

#endif	/* CHATREQUEST_HPP */

