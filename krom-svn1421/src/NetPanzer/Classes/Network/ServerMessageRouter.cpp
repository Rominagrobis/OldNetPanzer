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

#include "ServerMessageRouter.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/TeamManager.hpp"

#include "NetMessage.hpp"
#include "SystemNetMessage.hpp"
#include "ConnectNetMessage.hpp"
#include "PlayerNetMessage.hpp"
#include "Util/Log.hpp"

#include "Network/PlayerRequests/PlayerRequests.hpp"
#include "Network/PlayerRequests/AttackUnitRequest.hpp"
#include "Network/PlayerRequests/ManualFireRequest.hpp"
#include "Network/PlayerRequests/MoveUnitRequest.hpp"
#include "Network/PlayerRequests/ChangeObjectiveGenerationRequest.hpp"
#include "Network/PlayerRequests/ChangeObjectiveOutLocationRequest.hpp"
#include "Network/PlayerRequests/ChatRequest.hpp"
#include "Network/PlayerRequests/TeamChatRequest.hpp"
#include "Network/PlayerRequests/ChangeFlagRequest.hpp"
#include "Network/PlayerRequests/AllianceRequest.hpp"
#include "Network/PlayerRequests/BreakAllianceRequest.hpp"
#include "Network/PlayerRequests/ChangeTeamRequest.hpp"
#include "Interfaces/VoteManager.hpp"
#include "Network/PlayerRequests/VoteSelectedRequest.hpp"


NetPacket ServerMessageRouter::temp_packet;
NetMessageDecoder ServerMessageRouter::message_decoder;

void ServerMessageRouter::initialize()
{
}

void ServerMessageRouter::cleanUp()
{
}

static void processPlayerCommands(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    
    switch ( message->message_id )
    {
        case PlayerRequests::MOVE_UNIT :
            UnitInterface::playerCommand_MoveUnit( packet->fromPlayer,
                                        ((MoveUnitRequest*)message)->getUnitId(),
                                        ((MoveUnitRequest*)message)->getMapPos());
            break;
            
        case PlayerRequests::ATTACK_UNIT :
            UnitInterface::playerCommand_AttackUnit( packet->fromPlayer,
                                        ((AttackUnitRequest*)message)->getUnitId(),
                                        ((AttackUnitRequest*)message)->getEnemyId());
            break;
            
        case PlayerRequests::MANUAL_FIRE :
            UnitInterface::playerCommand_ManualShoot( packet->fromPlayer,
                                        ((ManualFireRequest*)message)->getUnitId(),
                                        ((ManualFireRequest*)message)->getMapPos());
            break;
            
        case PlayerRequests::CHANGE_OBJECTIVE_GENERATION :
            ObjectiveInterface::playerRequest_setGeneration(packet->fromPlayer,
                                        ((ChangeObjectiveGenerationRequest*)message)->getObjectiveId(),
                                        ((ChangeObjectiveGenerationRequest*)message)->unit_type,
                                        ((ChangeObjectiveGenerationRequest*)message)->unit_gen_on);
            break;
            
        case PlayerRequests::CHANGE_OBJECTIVE_OUTLOC :
            ObjectiveInterface::playerRequest_setOutputLoc(packet->fromPlayer,
                                        ((ChangeObjectiveOutLocationRequest*)message)->getObjectiveId(),
                                        ((ChangeObjectiveOutLocationRequest*)message)->getMapPos());
            break;
            
        case PlayerRequests::CHAT :
            if ( packet->size > sizeof(NetMessage) )
            {
                NPString text;
                ((ChatRequest*)message)->getText(packet->size, text);
                ChatInterface::playerRequest_chat(packet->fromPlayer, text);
            }
            else
            {
                LOGGER.warning("Player %d sent empty chat request", packet->fromPlayer);
            }
            break;
            
        case PlayerRequests::TEAM_CHAT :
            if ( packet->size > sizeof(NetMessage) )
            {
                NPString text;
                ((TeamChatRequest*)message)->getText(packet->size, text);
                ChatInterface::playerRequest_teamChat(packet->fromPlayer, text);
            }
            else
            {
                LOGGER.warning("Player %d sent empty team chat request", packet->fromPlayer);
            }
            break;
            
        case PlayerRequests::CHANGE_FLAG :
            PlayerInterface::playerRequest_changeFlag(packet->fromPlayer,
                                        ((ChangeFlagRequest*)message)->player_flag);
                                         
            break;
            
        case PlayerRequests::ALLIANCE_REQUEST :
            PlayerInterface::playerRequest_allianceRequest(packet->fromPlayer,
                                        ((AllianceRequest*)message)->with_player_id);
                                         
            break;
            
        case PlayerRequests::BREAK_ALLIANCE :
            PlayerInterface::playerRequest_breakAlliance(packet->fromPlayer,
                                        ((BreakAllianceRequest*)message)->with_player_id);
                                         
            break;
            
        case PlayerRequests::PLAYER_READY :
            TeamManager::playerRequest_ready(packet->fromPlayer);
            break;
            
        case PlayerRequests::CHANGE_TEAM :
            TeamManager::playerRequest_changeTeam(packet->fromPlayer,
                                        ((ChangeTeamRequest*)message)->team_id);
            break;
            
        case PlayerRequests::STARTSURRENDER_VOTE :
            VoteManager::playerRequest_startSurrenderVote(packet->fromPlayer);
            break;
            
        case PlayerRequests::VOTE_SELECTED :
            VoteManager::playerRequest_voteSelected(packet->fromPlayer,
                                        ((VoteSelectedRequest*)message)->vote_selected);
            break;
    }
}


void ServerMessageRouter::routePacket(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    PlayerState * player = PlayerInterface::getPlayer(packet->fromPlayer);

    switch (message->message_class) {
        case _net_message_class_player_commands:
            if ( player )
            {
                player->resetAutokick();
            }
            processPlayerCommands(packet);
            break;

        case _net_message_class_connect:
            ServerConnectDaemon::processNetPacket(packet);
            break;

        default:
            LOGGER.warning("Packet contained unknown message class: %d",
                    message->message_class);
    }
}

void ServerMessageRouter::routeMessages()
{
    ServerConnectDaemon::connectProcess();
    Uint16 msg_len;
    NetMessage* mmessage;

    while(SERVER->getPacket(&temp_packet) == true)
    {
        const NetMessage* message = temp_packet.getNetMessage();
        if (message->message_class == _net_message_class_multi)
        {
            message_decoder.setDecodeMessage((const MultiMessage *) message, temp_packet.size);

            NetPacket packet;
            packet.fromPlayer = temp_packet.fromPlayer;
            packet.fromClient = temp_packet.fromClient;
            
            while ( (msg_len = message_decoder.decodeMessage(&mmessage)) )
            {
                memcpy(packet.data, mmessage, msg_len);
                routePacket(&packet);
            }
        }
        else
        {
            routePacket(&temp_packet);
        }
    }
}
