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
#ifndef _TEAMMANAGER_HPP
#define _TEAMMANAGER_HPP

#include "Interfaces/Team.hpp"
#include "Classes/Network/NetMessage.hpp"

class TeamManager
{
protected:
    static Team *Teams_lists;
    static Uint8 max_Teams;

public:

    static void initialize(const Uint8 _max_teams);
    static Uint8 getTeamColor(TeamID team_id);
    static void reset();
    static void addPlayer(PlayerID player_id);
    static void addPlayerinTeam(PlayerID player_id, TeamID team_id);
    static void BalancedTeam();
    static void removePlayer(PlayerID player_id, TeamID team_id);
    static int CountPlayerinTeam(TeamID team_id);
    static void cleanUp();

    static PlayerID getMaxteams()
    {
        return max_Teams;
    }
    static iXY getPlayerSpawnPoint(PlayerID player_id);

    static short getKills( TeamID team_id );
    static short getLosses( TeamID team_id );
    static short getObjectivesHeld( TeamID team_id );
    static void incKills( TeamID team_id );
    static void incLosses( TeamID team_id );
    static void lockTeamStats();
    static void unlockTeamStats();
    static void resetTeamStats();
    static void sendScores();
    static void receiveScores(const NetMessage* message);

    static Uint8 getTeamWin();
    static const std::string& getTeamName( TeamID team_id );
    static bool testRuleScoreLimit( long score_limit );
    static bool testRuleObjectiveRatio( float precentage );
    static void drawFlag( TeamID team_id, Surface &dest, int x, int y);

    static void PlayerrequestchangeTeam(PlayerID player_id, TeamID newteam);
    static void PlayerchangeTeam(PlayerID player_id, TeamID team_idx);
    static void SpawnTeams();

    static bool CheckisPlayerReady();
    static bool isPlayerReady(PlayerID player_id);
    static void PlayerRequestReadyAccepted(PlayerID player_id);

    static void netMessageChangeTeamRequest(const NetMessage* message);
    static void serversayToTeam(const TeamID teamID, const NPString& message);
    static void sendMessageToTeam(const TeamID teamID, NetMessage* message, size_t size);

    static void playerRequest_ready(const PlayerID player_id);
    static void playerRequest_changeTeam(const PlayerID player_id, const TeamID team_id);
    
    static void handlePlayerTeamUpdate(const PlayerID player_id, const TeamID team_id);
    static void handlePlayerReadyUpdate(const PlayerID player_id);
    
};

#endif // ** _TEAMMANAGER_HPP
