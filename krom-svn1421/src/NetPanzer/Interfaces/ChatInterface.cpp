/*
 Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue

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

#include "2D/Color.hpp"

#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/StrManager.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Util/Log.hpp"
#include "Views/Game/ChatView.hpp"
#include "Views/Components/Desktop.hpp"

#include "Scripts/ScriptManager.hpp"

#include "Network/PlayerRequests/ChatRequest.hpp"
#include "Network/PlayerRequests/TeamChatRequest.hpp"

enum { _net_message_id_chat_mesg };

enum { _chat_mesg_scope_alliance,
       _chat_mesg_scope_all,
       _chat_mesg_scope_server
     };
     
enum {
    message_type_server,
    message_type_all,
    message_type_alliance
};

class ServerChatMessage : public ChatRequest
{
public:
    ServerChatMessage()
    {
        message_class = _net_message_class_chat;
        message_id = message_type_server;
    }
    
} __attribute__((packed));
     
class AllChatMessage : public NetMessage
{
public:
    enum { MAX_CHAT_LENGTH = _MAX_NET_PACKET_SIZE - 16 };
    enum { CHATMESG_HEADSIZE = sizeof(NetMessage) + sizeof(PlayerID) };
    
    PlayerID player_id;
    char message_text[MAX_CHAT_LENGTH];

    AllChatMessage()
    {
        message_class = _net_message_class_chat;
        message_id = message_type_all;
    }
    
    unsigned int setText(const NPString& text)
    {
        unsigned int text_len = std::min((unsigned int)text.size(), (unsigned int)MAX_CHAT_LENGTH);
        text.copy(message_text, text_len);
        
        return text_len + CHATMESG_HEADSIZE;
    }
    
    void getText(int packet_length, NPString& text) const
    {
        unsigned int text_len = packet_length - CHATMESG_HEADSIZE;
        text.assign(message_text, text_len);
    }

} __attribute__((packed));

class AllianceChatMessage : public AllChatMessage
{
public:
    AllianceChatMessage()
    {
        message_id = message_type_alliance;
    }
};

void ChatInterface::clientHandleChatMessage(const NetMessage* message, size_t size)
{
    NPString text;
    
    if ( message->message_id == message_type_server )
    {
        ((ServerChatMessage*)message)->getText(size, text);
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "Server: %s", text.c_str());
        return;
    }
    
    PlayerID from_player = ((AllChatMessage*)message)->player_id;
    if ( !PlayerInterface::isValidPlayerID(from_player) )
    {
        LOGGER.warning("Received chat message contains incorrect player value: %u", from_player);
        return;
    }
    
    ((AllChatMessage*)message)->getText(size, text);

    PlayerState *player_state;
    player_state = PlayerInterface::getPlayer(from_player);

    PIX color = (message->message_id == message_type_all) ? Color::white : Color::yellow;

    LOGGER.debug("C: %s: %s", player_state->getName().c_str(), text.c_str());

    ChatView *v = (ChatView*) Desktop::getView("ChatView");
    if (v)
        v->postMessage(color, true, player_state->getFlag(),
                       "%s: %s", player_state->getName().c_str(), text.c_str());
}

void ChatInterface::say(const NPString& message)
{
    ChatRequest req;
    int to_send = req.setText(message);
    
    CLIENT->sendMessage(&req, to_send);
}

void ChatInterface::teamsay(const NPString& message)
{
    TeamChatRequest req;
    int to_send = req.setText(message);
    
    CLIENT->sendMessage(&req, to_send);
}

void ChatInterface::serversay(const NPString& message)
{
    if ( NetworkState::status == _network_state_server )
    {
        ServerChatMessage msg;
        int to_send = msg.setText(message);
        
        SERVER->broadcastMessage(&msg, to_send);
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "Server: %s", message.c_str());
    }
}

void ChatInterface::serversayTo(const PlayerID player, const NPString& message)
{
    if ( ! PlayerInterface::isValidPlayerID(player) )
    {
        LOGGER.warning("serversayTo invalid player: %u", player);
    }

    if ( ! PlayerInterface::isPlayerActive(player) )
    {
        LOGGER.warning("serversayTo inactive player: %u", player);
    }

    if ( PlayerInterface::isLocalPlayer(player) )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, _("Server: %s"),
                                      message.c_str());
    }
    else
    {
        ServerChatMessage msg;
        int to_send = msg.setText(message);
        
        SERVER->sendMessage(player, &msg, to_send);
    }
}

static bool filterPlayerChat(const PlayerID player_id, const NPString& text, NPString& out)
{
    ScriptManager::prepareFunction("Filters.chatFilter");
    ScriptManager::pushParameterInt(player_id);
    ScriptManager::pushParameterString(text);
    if ( ScriptManager::callFunction(1) )
    {
        if ( ScriptManager::isNullResult(-1) || ! ScriptManager::getStringResult(-1, out) )
        {
            out.clear();
        }
    }
    else
    {
        NPString s;
        ScriptManager::getStringResult(-1, s);
        LOGGER.warning("Error in chatFilter: '%s'", s.c_str());
        out.assign(text);
    }
    
    ScriptManager::endFunction();
    
    return out.size() == 0;
}

void ChatInterface::playerRequest_chat(const PlayerID player_id, const NPString& message)
{
    NPString text;
    
    if ( filterPlayerChat(player_id, message, text) )
    {
        LOGGER.warning("Message from [%d] filtered", player_id);
        return;
    }

    if ( text[0] != '/' || ! ScriptManager::runServerCommand(text.substr(1), player_id) )
    {
        AllChatMessage msg;
        msg.player_id = player_id;
        int to_send = msg.setText(text);
        
        SERVER->broadcastMessage(&msg, to_send);
        
        PlayerState *player_state = PlayerInterface::getPlayer(player_id);
        LOGGER.debug("C: %s: %s", player_state->getName().c_str(), text.c_str());

        ChatView *v = (ChatView*) Desktop::getView("ChatView");
        if ( v )
        {
            v->postMessage(Color::white, true, player_state->getFlag(),
                           "%s: %s", player_state->getName().c_str(), text.c_str());
        }

    }    
}

void ChatInterface::playerRequest_teamChat(const PlayerID player_id, const NPString& message)
{
    AllianceChatMessage msg;
    msg.player_id = player_id;
    int to_send = msg.setText(message);
    
    PlayerID max_players = PlayerInterface::getMaxPlayers();
    PlayerID local_player_index = PlayerInterface::getLocalPlayerIndex();
    
    bool post_on_server = false;

    for (PlayerID i = 0; i < max_players; ++i)
    {
        if ( PlayerInterface::isAllied( player_id, i) == true )
        {
            if ( local_player_index != i )
            {
                SERVER->sendMessage(i, &msg, to_send);
            }
            else
            {
                // one of the allied players is me, non dedicated server
                post_on_server = true;
            }
        }
    }

    if ( player_id == local_player_index )
    {
        // non dedicated server sent the msg
        post_on_server = true;
    }
    else
    {
        SERVER->sendMessage(player_id, &msg, to_send);
    }
    
    if ( post_on_server == true )
    {
        PlayerState *player_state;

        player_state = PlayerInterface::getPlayer(player_id);

        LOGGER.debug("C: %s: %s", player_state->getName().c_str(), message.c_str());
        
        ChatView *v = (ChatView*) Desktop::getView("ChatView");
        if (v)
            v->postMessage(Color::yellow, true, player_state->getFlag(),
                           "%s: %s", player_state->getName().c_str(), message.c_str());
    }
    
}

