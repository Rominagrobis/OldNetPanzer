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

#include <stdexcept>

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/VoteManager.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Interfaces/StrManager.hpp"
#include "Units/UnitInterface.hpp"
#include "Resources/ResourceManager.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"

#include "Views/Components/Desktop.hpp"

#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Util/Log.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Network/PlayerRequests/ChangeFlagRequest.hpp"

class AllianceCreatedUpdate : public NetMessage
{
public:
    PlayerID by_player;
    PlayerID with_player;
    
    AllianceCreatedUpdate()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_alliance_created_update;
    }
} __attribute__((packed));

class AllianceBrokenUpdate : public NetMessage
{
public:
    PlayerID by_player;
    PlayerID with_player;
    
    AllianceBrokenUpdate()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_alliance_broken_update;
    }
} __attribute__((packed));

// ** PlayerInterface Statics
PlayerState   * PlayerInterface::player_lists = 0;
PlayerID        PlayerInterface::max_players = MIN_PLAYER_ID;

static bool   * alliance_matrix = 0; // XXX ALLY
PlayerID  PlayerInterface::local_player_index = INVALID_PLAYER_ID;

PlayerID PlayerInterface::respawn_rule_player_index = INVALID_PLAYER_ID;

SDL_mutex* PlayerInterface::mutex = 0;

static void setAlliance(PlayerID by_player, PlayerID with_player )
{
    *(alliance_matrix + (by_player * PlayerInterface::getMaxPlayers()) + with_player ) = true;
}

static void clearAlliance(PlayerID by_player, PlayerID with_player )
{
    *(alliance_matrix + (by_player * PlayerInterface::getMaxPlayers()) + with_player ) = false;
}

static void disconnectedPlayerAllianceCleanup(PlayerID index )
{
    for ( PlayerID player_index = 0; player_index < PlayerInterface::getMaxPlayers(); ++player_index )
    {
        clearAlliance( index, player_index );
        clearAlliance( player_index, index );
    }
}

bool PlayerInterface::isAllied( PlayerID player, PlayerID with_player )
{
    if ( player < max_players && with_player < max_players
            && *(alliance_matrix + (with_player * max_players) + player)
            && *(alliance_matrix + (player * max_players) + with_player) )
    {
        return true;
    }
    return false;
}

bool PlayerInterface::isSingleAllied( PlayerID player, PlayerID with_player )
{
    if ( player < max_players && with_player < max_players
            && *(alliance_matrix + (player * max_players) + with_player) )
    {
        return true;
    }
    return false;
}

static void resetAllianceMatrix()
{
    int matrix_size;

    matrix_size = PlayerInterface::getMaxPlayers() * PlayerInterface::getMaxPlayers();

    for ( int i = 0; i < matrix_size; i++ )
    {
        alliance_matrix[ i ] = false;
    }
}

void PlayerInterface::allyplayers(PlayerID by_player, PlayerID with_player )
{
    setAlliance( by_player, with_player );
    setAlliance( with_player, by_player );
}

void PlayerInterface::unallyplayers(PlayerID by_player, PlayerID with_player )
{
    clearAlliance( by_player, with_player );
    clearAlliance(with_player, by_player);
}

void PlayerInterface::initialize(const unsigned int _max_players)
{
    char temp_str[64];
    PlayerID player_id;
    max_players = _max_players;

    delete[] player_lists;
    player_lists = new PlayerState[max_players];

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].setID( player_id );
        player_lists[ player_id ].resetStats(false);
        player_lists[ player_id ].setStateFree();
        sprintf( temp_str, "Player %u", player_id );
        player_lists[ player_id ].setName( temp_str );
    }

    delete[] alliance_matrix;
    alliance_matrix = new bool [max_players * max_players];
    resetAllianceMatrix();

    mutex = SDL_CreateMutex();
    if(!mutex)
    {
        throw std::runtime_error("Couldn't create PlayerInterface mutex.");
    }
}

void PlayerInterface::reset()
{
    resetPlayerStats(countPlayers() > 0);
    resetAllianceMatrix(); // XXX ALLY
}

void PlayerInterface::cleanUp()
{
    delete[] player_lists;
    player_lists = 0;
    delete[] alliance_matrix;
    alliance_matrix = 0;
    max_players = 0;

    SDL_DestroyMutex(mutex);
    mutex = 0;
}

void PlayerInterface::lock()
{
    SDL_mutexP(mutex);
}

void PlayerInterface::unLock()
{
    SDL_mutexV(mutex);
}

void PlayerInterface::setKill(PlayerState* by_player, PlayerState* on_player,
                              UnitType unit_type)
{
    if (GameControlRulesDaemon::getGameState() == _game_state_completed) return;
    SDL_mutexP(mutex);
    by_player->incKills( unit_type );
    on_player->incLosses( unit_type );
    if (GameConfig::game_teammode)
    {
        TeamManager::incKills(by_player->getTeamID());
        TeamManager::incLosses(on_player->getTeamID());
    }
    SDL_mutexV(mutex);
}

void PlayerInterface::lockPlayerStats()
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].lockStats();
    } // ** for
    if (GameConfig::game_teammode) TeamManager::lockTeamStats();
    SDL_mutexV(mutex);
}

void PlayerInterface::unlockPlayerStats()
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].unlockStats();
    } // ** for
    if (GameConfig::game_teammode) TeamManager::unlockTeamStats();
    SDL_mutexV(mutex);
}

void PlayerInterface::resetPlayerStats(bool keepAdmin)
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].resetStats(keepAdmin);
    } // ** for
    if (GameConfig::game_teammode) TeamManager::resetTeamStats();
    SDL_mutexV(mutex);
}

int PlayerInterface::getActivePlayerCount()
{
    PlayerID player_id;
    int count = 0;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].isActive() && count++;
    } // ** for

    return( count );
}

PlayerState* PlayerInterface::allocateLoopBackPlayer()
{
    local_player_index = 0;

    SDL_mutexP(mutex);
    player_lists[local_player_index].setStateSelectingFlag();
    player_lists[local_player_index].unit_config.initialize();
    SDL_mutexV(mutex);

    return &player_lists[local_player_index];
}

PlayerID PlayerInterface::countPlayers()
{
    PlayerID count;
    PlayerID player_id;
    for ( player_id = 0, count=0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].isActive() && count++;
    }
    return count;
}

PlayerState * PlayerInterface::allocateNewPlayer()
{
    PlayerID player_id;
    PlayerState * res = 0;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( player_lists[ player_id ].isFree() )
        {
            player_lists[ player_id ].setStateAllocated();
            player_lists[ player_id ].resetStats(false);
            player_lists[ player_id ].unit_config.initialize();
            res = &player_lists[ player_id ];
            break;
        }
    }
    SDL_mutexV(mutex);
    return( res );
}

void PlayerInterface::spawnPlayer( PlayerID player_id, const iXY &location )
{
    if ( player_id < max_players )
    {
        SDL_mutexP(mutex);
        if ( ! player_lists[player_id].isFree() )
        {
            UnitInterface::spawnPlayerUnits( location,
                                             player_id,
                                             player_lists[ player_id ].unit_config
                                           );
        } // ** if _player_state_active
        SDL_mutexV(mutex);
    }
}


bool PlayerInterface::testRuleScoreLimit( long score_limit, PlayerState ** player_state )
{
    PlayerID player_id;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( player_lists[ player_id ].getKills() >= score_limit )
        {
            *player_state = &player_lists[ player_id ];
            return( true );
        } // ** if

    } // ** for

    return( false );
}

static bool testRuleObjectiveOccupationRatio( PlayerID player_id,
        float precentage )
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();

    size_t occupation_ratio = (size_t) ( ((float) num_objectives) * precentage  + 0.999);

    if (occupation_ratio == 0)
    {
        occupation_ratio = 1;
    }

    size_t occupied = 0;
    for ( int i = 0; i < num_objectives; ++i )
    {
        Objective *objective_state = ObjectiveInterface::getObjective(i);

        if ( objective_state->occupying_player != 0 )
        {
            PlayerID occuping_player_id = objective_state->occupying_player->getID();

            if ( occuping_player_id == player_id )
            {
                occupied++;
            }
            // XXX ALLY
            else if ( PlayerInterface::isAllied(occuping_player_id, player_id) )
            {
                occupied++;
            }
        }
    }

    if ( occupied >= occupation_ratio )
    {
        return true;
    }

    return false;
}

bool PlayerInterface::testRuleObjectiveRatio( float precentage, PlayerState ** player_state )
{
    PlayerID player_id;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( testRuleObjectiveOccupationRatio(player_id, precentage) )
        {
            *player_state = &player_lists[ player_id ];
            return true;
        } // ** if
    } // ** for

    return false;
}


bool PlayerInterface::testRulePlayerRespawn( bool *completed, PlayerState **player_state )
{
    if ( respawn_rule_player_index == max_players
            || respawn_rule_player_index == INVALID_PLAYER_ID )
    {
        respawn_rule_player_index = 0;
        *completed = true;
        return( false );
    }
    else
    {
        *completed = false;
    }

    if (  player_lists[ respawn_rule_player_index ].isPlaying()
            && UnitInterface::getUnitCount( respawn_rule_player_index ) == 0 )
    {
        *player_state = &player_lists[ respawn_rule_player_index ];
        ++respawn_rule_player_index;
        return( true );
    }

    ++respawn_rule_player_index;
    return( false );
}

void PlayerInterface::netMessageConnectID(const NetMessage* message)
{
    const PlayerConnectID *connect_mesg
    = (const PlayerConnectID *) message;

    local_player_index = connect_mesg->player_id;
    if(local_player_index >= max_players)
    {
        LOGGER.warning("Invalid netMessageConnectID Message");
        return;
    }

//    SDL_mutexP(mutex);
//    player_lists[local_player_index].setFromNetworkPlayerState
//        (&connect_mesg->connect_state);
//    SDL_mutexV(mutex);
}

void PlayerInterface::netMessageSyncState(const NetMessage* message)
{
    const PlayerStateSync *sync_mesg
    = (const PlayerStateSync *) message;
    PlayerID player_id = sync_mesg->player_state.getPlayerIndex();

    if(player_id >= max_players)
    {
        LOGGER.warning("Malformed MessageSyncState message");
        return;
    }

    SDL_mutexP(mutex);
    player_lists[player_id].setFromNetworkPlayerState(&sync_mesg->player_state);
    // XXX ALLY
    if ( player_lists[player_id].isFree() )
    {
        disconnectedPlayerAllianceCleanup(player_id);
    }
    if ( player_id == local_player_index && player_lists[player_id].isSelectingFlag() )
    {
        Desktop::setVisibility("GFlagSelectionView", true);
    }
    SDL_mutexV(mutex);
}

void PlayerInterface::netMessageScoreUpdate(const NetMessage *message)
{
    const PlayerScoreUpdate* score_update
    = (const PlayerScoreUpdate *) message;

    if(score_update->getKillByPlayerIndex() >= PlayerInterface::getMaxPlayers()
            || score_update->getKillOnPlayerIndex()
            >= PlayerInterface::getMaxPlayers())
    {
        LOGGER.warning("Malformed score update packet.");
        return;
    }

    PlayerState* player1 = getPlayer(score_update->getKillByPlayerIndex());
    PlayerState* player2 = getPlayer(score_update->getKillOnPlayerIndex());
    setKill(player1, player2, (UnitType) score_update->unit_type );
}

void PlayerInterface::processNetMessage(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    switch(message->message_id)
    {
    case _net_message_id_player_connect_id :
        if ( NetworkState::getNetworkStatus() == _network_state_client )
        {
            netMessageConnectID(message);
        }
        else
        {
            LOGGER.warning("### Player %d @ %s is trying to cheat!!!",
                            packet->fromPlayer,
                            packet->fromClient->getFullIPAddress().c_str() );
        }
        break;

    case _net_message_id_player_sync_flag:
    {
        const PlayerFlagSync* pfs = (const PlayerFlagSync*)message;
        ResourceManager::updateFlagData(pfs->player_id,
                                        pfs->player_flag,
                                        sizeof(pfs->player_flag) );

        player_lists[pfs->player_id].setStateActive();
    }
    break;

    case _net_message_id_player_sync_state :
        netMessageSyncState(message);
        break;

    case _net_message_id_player_score_update :
            netMessageScoreUpdate(message);
        break;

    case _net_message_id_team_change_update :
        TeamManager::handlePlayerTeamUpdate(((PlayerTeamUpdate*)message)->player_id, ((PlayerTeamUpdate*)message)->team_id);
        break;

    case _net_message_id_player_ready_update :
        TeamManager::handlePlayerReadyUpdate(((PlayerReadyUpdate*)message)->player_id);
        break;

    case _net_message_id_player_team_score_sync :
        if (GameControlRulesDaemon::getGameState() != _game_state_completed)
            TeamManager::receiveScores(message);
        break;

    case _net_message_id_player_vote_request :
        VoteManager::netMessageReceiveRequestVote(message);
        break;

    case _net_message_id_player_flagtimer_update :
        gameconfig->game_changeflagtime = ((const PlayerFlagTimerUpdate*)message)->getflagtimer();
        break;
        
    case _net_message_id_alliance_created_update :
        handleAllianceCreatedUpdate( ((AllianceCreatedUpdate*)message)->by_player,
                                     ((AllianceCreatedUpdate*)message)->with_player );
        break;
        
    case _net_message_id_alliance_broken_update :
        handleAllianceBrokenUpdate( ((AllianceBrokenUpdate*)message)->by_player,
                                    ((AllianceBrokenUpdate*)message)->with_player );
        break;

    }
}

void PlayerInterface::disconnectPlayerCleanup( PlayerID player_id )
{
    PlayerState *player_state = getPlayer( player_id );
    if ( player_state )
    {
        SDL_mutexP(mutex);

        // XXX ALLY
        disconnectedPlayerAllianceCleanup(player_id);

        player_state->setStateFree();
        player_state->setAdmin(false);

        PlayerStateSync player_state_update(player_state->getNetworkPlayerState());

        SDL_mutexV(mutex);

        SERVER->broadcastMessage(&player_state_update, sizeof(PlayerStateSync));
    }
}

void PlayerInterface::SyncFlagTimer()
{
    PlayerFlagTimerUpdate player_flagtimer_update(gameconfig->game_changeflagtime);

    SERVER->broadcastMessage(&player_flagtimer_update, sizeof(PlayerFlagTimerUpdate));
}
 
void PlayerInterface::playerRequest_changeFlag(const PlayerID player_id, const Uint8* data)
{
    ResourceManager::updateFlagData(player_id, data, FLAG_WIDTH*FLAG_HEIGHT );

    PlayerFlagSync pfs;
    pfs.player_id = player_id;
    
    memcpy(pfs.player_flag, data, sizeof(pfs.player_flag));

    player_lists[player_id].setStateActive();

    SERVER->broadcastMessage(&pfs, sizeof(pfs));
}

void PlayerInterface::playerRequest_allianceRequest(const PlayerID player_id, const PlayerID with_player_id)
{
    setAlliance(player_id, with_player_id);
 
    AllianceCreatedUpdate upd;
    upd.by_player = player_id;
    upd.with_player = with_player_id;
    
    if ( player_id != local_player_index )
    {
        SERVER->sendMessage(player_id, &upd, sizeof(upd));
    }
    else
    {
        handleAllianceCreatedUpdate(player_id, with_player_id);
    }
    
    if ( with_player_id != local_player_index )
    {
        SERVER->sendMessage(with_player_id, &upd, sizeof(upd));
    }
    else
    {
        handleAllianceCreatedUpdate(player_id, with_player_id);
    }

}

void PlayerInterface::playerRequest_breakAlliance(const PlayerID player_id, const PlayerID with_player_id)
{
    clearAlliance(player_id, with_player_id);

    AllianceBrokenUpdate upd;
    upd.by_player = player_id;
    upd.with_player = with_player_id;
    
    if ( player_id != local_player_index )
    {
        SERVER->sendMessage(player_id, &upd, sizeof(upd));
    }
    else
    {
        handleAllianceBrokenUpdate(player_id, with_player_id);
    }
    
    if ( with_player_id != local_player_index )
    {
        SERVER->sendMessage(with_player_id, &upd, sizeof(upd));
    }
    else
    {
        handleAllianceBrokenUpdate(player_id, with_player_id);
    }
    
    clearAlliance(with_player_id, player_id);
}

void PlayerInterface::handleAllianceCreatedUpdate(const PlayerID by_player, const PlayerID with_player)
{
    PlayerState * player_state;
    
    setAlliance(by_player, with_player);
 
    if ( by_player == local_player_index )
    {
        player_state = PlayerInterface::getPlayer(with_player);
        if ( PlayerInterface::isSingleAllied(with_player, by_player) )
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("You accepted %s alliance request."),
                                          player_state->getName().c_str());
        }
        else
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("You request alliance with %s."),
                                          player_state->getName().c_str());
        }
    }
    else if ( with_player == local_player_index )
    {
        player_state = PlayerInterface::getPlayer(by_player);
        if ( PlayerInterface::isSingleAllied( with_player, by_player) )
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("%s accepted your alliance request."),
                                          player_state->getName().c_str());
        }
        else
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("%s request to ally with you."),
                                          player_state->getName().c_str());
        }
    }
}

void PlayerInterface::handleAllianceBrokenUpdate(const PlayerID by_player, const PlayerID with_player)
{
    PlayerState * player_state;
    
    clearAlliance(by_player, with_player);
    
    if ( by_player == local_player_index )
    {
        player_state = PlayerInterface::getPlayer(with_player);
        if ( PlayerInterface::isSingleAllied(with_player, by_player) )
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("You broke the alliance with %s."),
                                          player_state->getName().c_str());
        }
        else
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("You cancel your alliance request with %s."),
                                          player_state->getName().c_str());
        }
    }
    else if ( with_player == local_player_index )
    {
        player_state = PlayerInterface::getPlayer(by_player);
        if ( PlayerInterface::isSingleAllied( with_player, by_player) )
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("%s broke the alliance with you."),
                                          player_state->getName().c_str());
        }
        else
        {
            ConsoleInterface::postMessage(Color::yellow, false, 0,
                                          _("%s canceled the alliance request with you."),
                                          player_state->getName().c_str());
        }
    }
    
    clearAlliance(with_player, by_player);
}
