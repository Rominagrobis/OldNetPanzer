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

#include "ObjectiveInterface.hpp"
#include "Objective.hpp"

#include <stdio.h>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "Interfaces/MapInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"

#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"
#include "Util/FileStream.hpp"
#include "Util/Log.hpp"

#include "Classes/Network/NetMessage.hpp"
#include "Classes/Network/ObjectiveNetMessage.hpp"
#include "Classes/Network/NetPacket.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetMessageEncoder.hpp"

#include "Network/ClientSocket.hpp"

#include "Units/UnitProfileInterface.hpp"

#include "Network/PlayerRequests/ChangeObjectiveOutLocationRequest.hpp"
#include "Network/PlayerRequests/ChangeObjectiveGenerationRequest.hpp"

class ObjectiveOutLocSync : public ChangeObjectiveOutLocationRequest
{
public:
    ObjectiveOutLocSync()
    {
        message_class = _net_message_class_objective;
        message_id = _net_message_id_objective_outloc_sync;
    }
} __attribute__((packed));

class ObjectiveWorkingSync : public ChangeObjectiveGenerationRequest
{
public:
    ObjectiveWorkingSync()
    {
        message_class = _net_message_class_objective;
        message_id = _net_message_id_objective_working_sync;
    }
} __attribute__((packed));

Objective** ObjectiveInterface::objective_list = 0;
int ObjectiveInterface::num_objectives = 0;

void
ObjectiveInterface::cleanUpObjectiveList()
{
    if ( objective_list )
    {
        for ( int n=0; n < num_objectives; ++n )
        {
            delete objective_list[n];
        }
        delete[] objective_list;
        objective_list = 0;
    }

    num_objectives = 0;
}

void
ObjectiveInterface::cleanUp()
{
    cleanUpObjectiveList();
}

void
ObjectiveInterface::resetLogic()
{
    cleanUpObjectiveList();
}

static inline std::string readToken(std::istream& in, std::string tokenname)
{
    if(in.eof())
        throw std::runtime_error("file too short.");
    
    // skip whitespace characters and comments
    char c;
    do {
        in.get(c);
        if(c == '#') {  // comment till end of line
            do {
                in.get(c);
            } while(!in.eof() && c != '\n');
        }
    } while(!in.eof() && isspace(c));

    if(in.eof())
        throw std::runtime_error("file too short.");

    // read token
    std::string token;
    while(!in.eof() && !isspace(c)) {
        token += c;
        in.get(c);
    }
    if(token != tokenname) {
        std::stringstream msg;
        msg << "Expected token '" << tokenname << "' got '" << token << "'.";
        throw std::runtime_error(msg.str());
    }

    while(!in.eof() && isspace(c))
        in.get(c);

    // read token contents
    std::string result;
    while(!in.eof() && c != '\n') {
        result += c;
        in.get(c);
    }

    return result;
}

void
ObjectiveInterface::loadObjectiveList(const char *file_path)
{
    if ( ! GameConfig::game_enable_bases )
    {
        return;
    }

    ObjectiveID objective_count = 0;

    try
    {
        IFileStream in(file_path);

        cleanUpObjectiveList();

        std::string objectivecount = readToken(in, "ObjectiveCount:");
        std::stringstream ss(objectivecount);
        ss >> objective_count;

        if ( objective_count > 0 )
        {
            objective_list = new Objective*[objective_count];
            num_objectives = 0;
        }

        iXY loc;
        iXY world;
        std::string name;

        for (ObjectiveID objective_index = 0; objective_index < objective_count; objective_index++ )
        {
            Objective *objective_obj;
       
            name = readToken(in, "Name:");
            std::string location = readToken(in, "Location:");
            std::stringstream ss(location);
            ss >> loc.x >> loc.y;
            
            MapInterface::mapXYtoPointXY( loc, world );

            objective_obj = new Objective(objective_index, world,
                    BoundBox( -48, -32, 48, 32 )
                    );
            
            strcpy(objective_obj->name, name.c_str());
            objective_list[objective_index] = objective_obj;
            ++num_objectives;
        } // ** for
    }
    catch(std::exception& e)
    {
        cleanUpObjectiveList();
        LOGGER.warning("OILOL Error loading objectives '%s': %s. Working without them",
                       file_path, e.what());
    }
}

void
ObjectiveInterface::playerRequest_setGeneration(const PlayerID player_id, const ObjectiveID objective_id, const Uint8 unit_type, const bool active)
{
    if ( objective_id >= num_objectives )
    {
        LOGGER.warning("OISH_CGU CHEAT Player %u sent invalid objective_id %u, max is %u",
                       player_id, objective_id, num_objectives);
        return;
    }

    if ( objective_list[objective_id]->occupying_player->getID() != player_id )
    {
        LOGGER.warning("OISH_CGU CHEAT Player %u doesn't own objective %u",
                       player_id, objective_id);
        return;
    }

    if ( unit_type >= UnitProfileInterface::getNumUnitTypes() )
    {
        LOGGER.warning("OISH_CGU CHEAT Player %u sent invalid unit type %u, max is %u",
                       player_id,
                       unit_type,
                       UnitProfileInterface::getNumUnitTypes());
        return;
    }

    objective_list[objective_id]->changeUnitGeneration(active, unit_type);

    ObjectiveWorkingSync msg;
    msg.setObjectiveId(objective_id);
    msg.unit_type = unit_type;
    msg.unit_gen_on = active;
    
    SERVER->sendMessage(player_id, &msg, sizeof(msg));
}


void ObjectiveInterface::playerRequest_setOutputLoc(const PlayerID player_id, const ObjectiveID objective_id, const iXY& location)
{
    if ( objective_id >= num_objectives )
    {
        LOGGER.warning("OISH_COL CHEAT Player %u sent invalid objective_id %u, max is %u",
                       player_id, objective_id, num_objectives);
        return;
    }

    if ( location.x >= MapInterface::getWidth()
         || location.y >= MapInterface::getHeight() )
    {
        LOGGER.warning("OISH_COL CHEAT Player %u sent invalid map location %u,%u; max is %u,%u",
                       player_id,
                       location.x, location.y,
                       (unsigned int)MapInterface::getWidth(),
                       (unsigned int)MapInterface::getHeight());
        return;
    }

    objective_list[objective_id]->unit_collection_loc = location;

    ObjectiveOutLocSync msg;
    msg.setObjectiveId(objective_id);
    msg.setMapPos(location);
    SERVER->sendMessage(player_id, &msg, sizeof(msg));
}

void
ObjectiveInterface::clientHandleNetMessage(const NetMessage* message)
{
    switch(message->message_id)
    {
        case _net_message_id_occupation_status_update:
        {
            const ObjectiveOccupationUpdate* msg =
                (const ObjectiveOccupationUpdate*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_OOU CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives);
                break;
            }

            PlayerID player_id = msg->getPlayerId();
            if ( player_id >= PlayerInterface::getMaxPlayers() && player_id != INVALID_PLAYER_ID )
            {
                LOGGER.warning("OICH_OOU CHEAT SERVER sent invalid player_id %u, max is %u",
                               player_id, PlayerInterface::getMaxPlayers());
                break;
            }
            
            PlayerState* player = 0;

            if ( player_id != INVALID_PLAYER_ID )
            {
                player = PlayerInterface::getPlayer(player_id);
                if ( player && ! player->isActive() )
                {
                    LOGGER.warning("OICH_OOU CHEAT SERVER sent inactive player_id %u",
                                   player_id);
                    break;
                }
            }

            objective_list[obj_id]->changeOwner(player);

            break;
        }

        case _net_message_id_objective_sync:
        {
            const ObjectiveSyncMesg* msg =
                (const ObjectiveSyncMesg*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_OSM CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives);
                break;
            }

            objective_list[obj_id]->syncFromData(msg->sync_data);

            break;
        }

        case _net_message_id_objective_working_sync:
        {
            const ObjectiveWorkingSync* msg =
                (const ObjectiveWorkingSync*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_CGU CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives-1);
                break;
            }

            if ( msg->unit_type >= UnitProfileInterface::getNumUnitTypes() )
            {
                LOGGER.warning("OICH_CGU CHEAT SERVER sent invalid unit type %u, max is %u",
                               msg->unit_type,
                               UnitProfileInterface::getNumUnitTypes());
                break;
            }

            objective_list[obj_id]->changeUnitGeneration(msg->unit_gen_on, msg->unit_type);

            break;
        }

        case _net_message_id_objective_outloc_sync:
        {
            const ObjectiveOutLocSync* msg =
                (const ObjectiveOutLocSync*) message;

            ObjectiveID obj_id = msg->getObjectiveId();
            if ( obj_id >= num_objectives )
            {
                LOGGER.warning("OICH_COL CHEAT SERVER sent invalid objective_id %u, max is %u",
                               obj_id, num_objectives-1);
                break;
            }

            iXY location = msg->getMapPos();
            
            if ( location.x >= MapInterface::getWidth()
                 || location.y >= MapInterface::getHeight() )
            {
                LOGGER.warning("OICH_COL CHEAT SERVER sent invalid map location %u,%u; max is %u,%u",
                               location.x, location.y,
                               (unsigned int)MapInterface::getWidth(),
                               (unsigned int)MapInterface::getHeight());
                break;
            }

            objective_list[obj_id]->unit_collection_loc = location;
            break;
        }
        
        default:
            LOGGER.warning("OICH CHEAT SERVER sent unknown message type %u",
                           message->message_id);
    }
}

void
ObjectiveInterface::updateObjectiveStatus()
{
    for ( int i = 0; i < num_objectives; ++i )
    {
        objective_list[i]->updateStatus();
    }
}

void
ObjectiveInterface::disownPlayerObjectives(PlayerID player_id)
{
    for(Uint16 i = 0; i < num_objectives; ++i)
    {
        if ( objective_list[i]->occupying_player
             && objective_list[i]->occupying_player->getID() == player_id )
        {
            objective_list[i]->changeOwner(0);

            ObjectiveOccupationUpdate update_mesg;
            update_mesg.set( i, INVALID_PLAYER_ID);
            SERVER->broadcastMessage(&update_mesg, sizeof(update_mesg));
        }
    }
}

Objective*
ObjectiveInterface::getObjectiveAtWorldXY(const iXY& loc)
{
    for ( Uint16 i = 0; i < num_objectives; ++i )
    {
        if ( objective_list[i]->selection_box.contains( loc ) )
        {
            return objective_list[i];
        }
    }
    return 0;
}

void
ObjectiveInterface::syncObjectives( ClientSocket * client )
{
    NetMessageEncoder encoder;
    ObjectiveSyncMesg msg;

    for ( int i = 0; i < num_objectives; ++i )
    {
        msg.set(i);
        objective_list[i]->getSyncData( msg.sync_data );

        if ( ! encoder.encodeMessage(&msg, sizeof(msg)) )
        {
            client->sendMessage(encoder.getEncodedMessage(),
                                encoder.getEncodedLen());
            encoder.resetEncoder();
            encoder.encodeMessage(&msg, sizeof(msg)); // this time shuold be good
        }
    }

    client->sendMessage(encoder.getEncodedMessage(),
                        encoder.getEncodedLen());
}

int
ObjectiveInterface::getObjectiveLimit()
{
    float percentage
        = (float) GameConfig::game_occupationpercentage / 100.0;
    int wincount = (int) ( ((float) getObjectiveCount()) * percentage + 0.999);
    if(wincount == 0)
        wincount = 1;

    return wincount;
}

