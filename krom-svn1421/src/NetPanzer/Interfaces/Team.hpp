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
#ifndef _TEAM_HPP
#define _TEAM_HPP

#include "Core/CoreTypes.hpp"
#include "2D/Surface.hpp"

class Team
{
protected:
    TeamID teamID;
    std::string name;
    Surface Flag;
    Uint8 teamColor;
    
    short kills;
    short losses;
    short total;
    bool stats_locked;

public:
    void initialize(const TeamID id);

    void setName(const std::string& newname);
    const std::string& getName() const;
    void setColor(const Uint8 newcolor);
    Uint8 getColor() const {return teamColor;}
    void addPlayer(PlayerID player_id);
    void removePlayer(PlayerID player_id);
    void cleanUp();
    TeamID getID() const { return teamID; }
    PlayerID countPlayers() const;
    PlayerID getrandomplayer() const;
    void loadFlag(const char *fileName);
    void drawFlag(Surface &dest, int x, int y) const;

    short getKills() const;
    short getLosses() const;
    short getTeamObjective() const;
    void incKills();
    void incLosses();
    void lockStats();
    void unlockStats();
    void resetStats();
    void syncScore(short newKills, short newLosses);
};

#endif // ** _TEAM_HPP
