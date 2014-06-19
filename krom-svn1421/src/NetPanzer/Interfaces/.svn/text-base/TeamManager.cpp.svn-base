/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Laurent Jacques

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

#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/Team.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/SystemNetMessage.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Util/Log.hpp"
#include "Util/StringUtil.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Game/PrepareTeam.hpp"
#include "Network/PlayerRequests/PlayerReadyRequest.hpp"

Team       * TeamManager::Teams_lists = 0;
Uint8        TeamManager::max_Teams = 0;
static bool   * player_ready = 0;

static void resetPlayerReady()
{
    for ( int i = 0; i < PlayerInterface::getMaxPlayers(); i++ )
    {
        player_ready[ i ] = false;
    }
}

static bool isAllPlayersReady()
{
    for ( int i = 0; i < PlayerInterface::getMaxPlayers(); i++ )
    {
        if (PlayerInterface::getPlayer(i)->isActive())
        {
            if (player_ready[ i ] == false) return false;
        }
    }
    return true;
}

static void setReady(PlayerID player_id)
{
    player_ready[ player_id ] = true;
}

bool TeamManager::isPlayerReady(PlayerID player_id)
{
    return player_ready[ player_id ];
}

void TeamManager::initialize(const Uint8 _max_teams)
{
    char txtBuf[256];
    Uint8 colorsteam[4] = {Color::yellow, 197, Color::green, Color::cyan};
    max_Teams = _max_teams;

    delete[] Teams_lists;
    Teams_lists = new Team[max_Teams];
    delete[] player_ready;
    player_ready = new bool [PlayerInterface::getMaxPlayers()];

    std::vector<NPString> plist;
    NPString pl = *GameConfig::game_team_names;
    string_to_params(pl, plist);
    reset();

    for ( int team_id = 0; team_id < max_Teams; ++team_id )
    {
        Teams_lists[ team_id ].initialize(team_id);
        if (team_id < (TeamID) plist.size()) Teams_lists[ team_id ].setName(plist[team_id]);
        snprintf(txtBuf, sizeof(txtBuf), "pics/default/team-%d.bmp", team_id+1);
        Teams_lists[ team_id ].loadFlag(txtBuf);
        Teams_lists[ team_id ].setColor(colorsteam[team_id]);
    }
}

Uint8 TeamManager::getTeamColor(TeamID team_id)
{
    return Teams_lists[ team_id ].getColor();
}

void TeamManager::addPlayer(PlayerID player_id)
{
    int team_id;
    int lowTeam = 0, countPlayers = Teams_lists[ 0 ].countPlayers();
    for ( team_id = 0; team_id < max_Teams; ++team_id )
    {
        if (Teams_lists[ team_id ].countPlayers() < countPlayers)
        {
            countPlayers = Teams_lists[ team_id ].countPlayers();
            lowTeam = team_id;
        }
    }
    Teams_lists[ lowTeam ].addPlayer(player_id);
}

int TeamManager::CountPlayerinTeam(TeamID team_id)
{
    return Teams_lists[ team_id ].countPlayers();
}

void TeamManager::BalancedTeam()
{
    int team_id;
    int lowTeam = 0, highTeam = 0;
    int minPlayers = Teams_lists[ 0 ].countPlayers(), maxPlayers = 0;

    for ( team_id = 0; team_id < max_Teams; ++team_id )
    {
        if (Teams_lists[ team_id ].countPlayers() < minPlayers)
        {
            minPlayers = Teams_lists[ team_id ].countPlayers();
            lowTeam = team_id;
        }
        if (Teams_lists[ team_id ].countPlayers() > maxPlayers)
        {
            maxPlayers = Teams_lists[ team_id ].countPlayers();
            highTeam = team_id;
        }
    }
    
    if ((maxPlayers - minPlayers) > 1)
    {
        playerRequest_changeTeam(Teams_lists[ highTeam ].getrandomplayer(), lowTeam);
    }
}

void TeamManager::reset()
{
    resetPlayerReady();
    resetTeamStats();
    if ( NetworkState::status == _network_state_client )
    {
        PrepareTeam *v = (PrepareTeam*)Desktop::getView("PrepareTeam");
        if ( v )
        {
            v->resetReady();
        }
    }
}

void TeamManager::addPlayerinTeam(PlayerID player_id, TeamID team_id)
{
    Teams_lists[ team_id ].addPlayer(player_id);
}

void TeamManager::removePlayer(PlayerID player_id, TeamID team_id)
{
    Teams_lists[ team_id ].removePlayer(player_id);
}

void TeamManager::cleanUp()
{
    TeamID team_id;
    for ( team_id = 0; team_id < max_Teams; ++team_id )
    {
        Teams_lists[ team_id ].cleanUp();
    }
    delete[] Teams_lists;
    Teams_lists = 0;
    max_Teams = 0;
}

iXY TeamManager::getPlayerSpawnPoint(PlayerID player_id)
{
    TeamID Team_id = PlayerInterface::getPlayer(player_id)->getTeamID();

    iXY spawn_point;
    switch (Team_id)
    {
    case 0:
        spawn_point = MapInterface::getMinSpawnPoint();
        break;
    case 1:
        spawn_point = MapInterface::getMaxSpawnPoint();
        break;
    default:
        spawn_point = MapInterface::getFreeSpawnPoint();
    }
    return spawn_point;
}

void TeamManager::lockTeamStats()
{
    for ( TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        Teams_lists[ team_id ].lockStats();
    }
}

void TeamManager::unlockTeamStats()
{
    for ( TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        Teams_lists[ team_id ].unlockStats();
    }
}

void TeamManager::resetTeamStats()
{
    for ( TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        Teams_lists[ team_id ].resetStats();
    }
}

short TeamManager::getKills(  TeamID team_id )
{
    return Teams_lists[team_id].getKills();
}

short TeamManager::getLosses(  TeamID team_id )
{
    return Teams_lists[team_id].getLosses();
}

void TeamManager::incKills( TeamID team_id )
{
    Teams_lists[team_id].incKills();
}

void TeamManager::incLosses( TeamID team_id )
{
    Teams_lists[team_id].incLosses();
}

short TeamManager::getObjectivesHeld( TeamID team_id )
{
    return Teams_lists[team_id].getTeamObjective();
}

TeamID TeamManager::getTeamWin()
{
    long Score = 0;
    TeamID teamWin = 0;
    unsigned char game_type = GameConfig::game_gametype;

    switch (game_type)
    {
    case _gametype_fraglimit:
    case _gametype_timelimit:
        for (TeamID team_id = 0; team_id < max_Teams; ++team_id )
        {
            if (Score < Teams_lists[team_id].getKills())
            {
                teamWin = team_id;
                Score = Teams_lists[team_id].getKills();
            }
        }
        break;
    case _gametype_objective:
        for (TeamID team_id = 0; team_id < max_Teams; ++team_id )
        {
            if (Score < Teams_lists[team_id].getTeamObjective())
            {
                teamWin = team_id;
                Score = Teams_lists[team_id].getTeamObjective();
            }
        }
        break;
    }
    return teamWin;
}

const std::string& TeamManager::getTeamName( TeamID team_id )
{
    return Teams_lists[team_id].getName();
}

void TeamManager::drawFlag(TeamID team_id, Surface& dest, int x, int y)
{
    Teams_lists[team_id].drawFlag(dest, x, y);
}

bool TeamManager::testRuleScoreLimit( long score_limit )
{
    for (TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        if ( Teams_lists[team_id].getKills() >= score_limit )
            return( true );
    }
    return( false );
}

bool TeamManager::testRuleObjectiveRatio( float precentage )
{
    ObjectiveID max_objectives = ObjectiveInterface::getObjectiveLimit();

    for (TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        if ( Teams_lists[team_id].getTeamObjective() >= max_objectives )
        {
            return true;
        }
    }
    return false;
}

bool TeamManager::CheckisPlayerReady()
{
    if (PlayerInterface::getActivePlayerCount() > 1)
    {
        if (isAllPlayersReady())
        {
            return true;
        }
    }
    return false;
}

void TeamManager::PlayerRequestReadyAccepted(PlayerID player_id)
{
    setReady(player_id);
}

void TeamManager::SpawnTeams()
{
    for ( PlayerID player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        PlayerState* state_id = PlayerInterface::getPlayer(player_id);
        if (state_id->isActive())
        {
            UnitInterface::destroyPlayerUnits(player_id);
            GameManager::spawnPlayer( player_id );
        }
    }
}

void TeamManager::sendScores()
{
    TeamScoreSync score_Sync;
    for (TeamID team_id = 0; team_id < max_Teams; ++team_id )
    {
        score_Sync.set(team_id, getKills(team_id), getLosses(team_id));
        SERVER->broadcastMessage(&score_Sync, sizeof(TeamScoreSync));
    }
}

void TeamManager::receiveScores(const NetMessage* message)
{
    const TeamScoreSync* score_Sync
    = (const TeamScoreSync *) message;

    if (score_Sync->TeamID < max_Teams)
        Teams_lists[score_Sync->TeamID].syncScore(score_Sync->getKills(), score_Sync->getLosses());
}

void TeamManager::serversayToTeam(const TeamID teamID, const NPString& message)
{
    PlayerID player_id;
    for ( player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        if (PlayerInterface::getPlayer(player_id)->isActive())
        {
            if (PlayerInterface::getPlayer(player_id)->getTeamID() == teamID)
            {
                ChatInterface::serversayTo(player_id, message);
            }
        }
    }
}

void TeamManager::sendMessageToTeam(const TeamID teamID, NetMessage* message, size_t size)
{
    PlayerID player_id;
    for ( player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        if (PlayerInterface::getPlayer(player_id)->isActive())
        {
            if (PlayerInterface::getPlayer(player_id)->getTeamID() == teamID)
            {
                SERVER->sendMessage(player_id, message, size);
            }
        }
    }
}

void TeamManager::playerRequest_ready(const PlayerID player_id)
{
    setReady(player_id);
    
    PlayerReadyUpdate msg;
    msg.player_id = player_id;
    SERVER->broadcastMessage( &msg, sizeof(msg));
}

void TeamManager::playerRequest_changeTeam(const PlayerID player_id, const TeamID team_id)
{
    TeamID current_team = PlayerInterface::getPlayer(player_id)->getTeamID();

    if (GameControlRulesDaemon::getGameState() == _game_state_prepare_team)
    {
        if (((PlayerInterface::getMaxPlayers()/2) > Teams_lists[team_id].countPlayers()))
        {
            Teams_lists[current_team].removePlayer(player_id);
            Teams_lists[team_id].addPlayer(player_id);
            
            PlayerTeamUpdate upd;
            upd.player_id = player_id;
            upd.team_id = team_id;
            
            SERVER->broadcastMessage( &upd, sizeof(upd));
        }
    }
    else
    {
        if ( (Teams_lists[team_id].countPlayers() < Teams_lists[current_team].countPlayers())
                && ((PlayerInterface::getMaxPlayers()/2) > Teams_lists[team_id].countPlayers()))
        {
            Teams_lists[current_team].removePlayer(player_id);
            Teams_lists[team_id].addPlayer(player_id);
            
            PlayerTeamUpdate upd;
            upd.player_id = player_id;
            upd.team_id = team_id;
            
            SERVER->broadcastMessage( &upd, sizeof(upd));
            
            ObjectiveInterface::disownPlayerObjectives(player_id);
            UnitInterface::destroyPlayerUnits(player_id);
            GameManager::spawnPlayer( player_id );
        }
    }
}

void TeamManager::handlePlayerTeamUpdate(const PlayerID player_id, const TeamID team_id)
{
    TeamID current_team = PlayerInterface::getPlayer(player_id)->getTeamID();

    Teams_lists[current_team].removePlayer(player_id);
    Teams_lists[team_id].addPlayer(player_id);

    if (GameControlRulesDaemon::getGameState() != _game_state_prepare_team)
    {
        ConsoleInterface::postMessage(Color::yellow, false, 0,
                                      "%s has changed to team %s.",
                                      PlayerInterface::getPlayer(player_id)->getName().c_str(),
                                      Teams_lists[team_id].getName().c_str());
    }
}

void TeamManager::handlePlayerReadyUpdate(const PlayerID player_id)
{
    setReady(player_id);
}
