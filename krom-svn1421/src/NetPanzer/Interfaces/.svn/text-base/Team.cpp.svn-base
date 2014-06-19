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

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/Team.hpp"
#include "Util/Log.hpp"
#include "2D/Surface.hpp"

void Team::initialize(const TeamID id)
{
    teamID = id;
    resetStats();
}

void Team::setName(const std::string& newname)
{
    if ( newname.length() > 15 )
    {
        name = newname.substr(0,15);
    }
    else
    {
        name = newname;
    }
}

const std::string& Team::getName() const
{
    return name;
}

void Team::setColor(const Uint8 newcolor)
{
    teamColor = newcolor;
}

void Team::addPlayer(PlayerID new_player)
{
    for ( PlayerID player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        PlayerState* state = PlayerInterface::getPlayer(player_id);
        if (state->isActive())
        {
            if (state->getTeamID() == teamID
                && (player_id != new_player)) 
            {
                PlayerInterface::allyplayers( player_id, new_player);
            }
        }
    }
    PlayerInterface::getPlayer(new_player)->setTeamID(teamID);
}

void Team::removePlayer(PlayerID old_player)
{
    for ( PlayerID player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        if (PlayerInterface::getPlayer(player_id)->isActive())
        {
            if (PlayerInterface::getPlayer(player_id)->getTeamID() == teamID) 
            {
                PlayerInterface::unallyplayers( old_player, player_id);
            }
        }
    }
    PlayerInterface::getPlayer(old_player)->setTeamID(0xFF);
}

void Team::cleanUp()
{
}

PlayerID Team::countPlayers() const
{
    PlayerID count = 0;
    PlayerID player_id;
    for ( player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        if (PlayerInterface::getPlayer(player_id)->isActive())
        {
            if (PlayerInterface::getPlayer(player_id)->getTeamID() == teamID) 
            {
                count++;
            }
        }
    }
    return count;
}

PlayerID Team::getrandomplayer() const
{
    PlayerID count = 0;
    PlayerID player_id, result =0;
    Uint8 player = rand()%countPlayers();
    
    for ( player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        if (PlayerInterface::getPlayer(player_id)->isActive())
        {
            if (PlayerInterface::getPlayer(player_id)->getTeamID() == teamID) 
            {
                if (count == player) return player_id;
                count++;
                result = player_id;
            }
        }
    }
    return result; // default return the last
}

void Team::loadFlag(const char *fileName)
{
    Flag.loadBMP(fileName);
}

void Team::drawFlag(Surface &dest, int x, int y) const
{
    Flag.bltTrans(dest, x, y);
}

short Team::getKills() const
{
    return kills;
}

short Team::getLosses() const
{
    return losses;
}

short Team::getTeamObjective() const
{
    short TeamObjective = 0;
    PlayerID player_id;
    
    for ( player_id = 0; player_id < PlayerInterface::getMaxPlayers(); ++player_id )
    {
        PlayerState* state = PlayerInterface::getPlayer(player_id);
        if (state->isActive())
        {
            if (state->getTeamID() == teamID) 
            {
                TeamObjective += state->getObjectivesHeld();
            }
        }
    }
    return TeamObjective;
}
void Team::incKills()
{
    if ( stats_locked == true )
        return;
    kills++;
}

void Team::incLosses()
{
    if ( stats_locked == true )
        return;
    losses++;
}

void Team::lockStats()
{
    stats_locked = true;
}

void Team::unlockStats()
{
    stats_locked = false;
}

void Team::resetStats()
{
    kills  = 0;
    losses = 0;
    stats_locked = false;
}

void Team::syncScore(short newKills, short newLosses)
{
    kills = newKills;
    losses = newLosses;
}

