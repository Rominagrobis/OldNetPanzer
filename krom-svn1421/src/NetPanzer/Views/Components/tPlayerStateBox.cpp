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


#include "Views/Components/tPlayerStateBox.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Resources/ResourceManager.hpp"

tPlayerStateBox::tPlayerStateBox(iRect rect, StateChangedCallback* newcallback)
        : tStringListBox(rect, newcallback)
{
    DrawFlags = true;
    UpdateState(true);
    ShowTeam = false;
    TeamNumber = 0;
}

int tPlayerStateBox::getMaxItemWidth(const DataItem& data)
{
    if (DrawFlags)
        return Surface::getTextLength(data.text)+34;
    else
        return Surface::getTextLength(data.text)+10;
}

void tPlayerStateBox::onPaint(Surface &dst, const DataItem& data)
{
    char statBuf[256];
    int StartX = 1;
    PlayerState *state = (PlayerState*)(data.Data);
    if (DrawFlags)
    {
        Surface * flag = 0;
        flag = ResourceManager::getFlag(state->getFlag());
        flag->blt(dst, 0, 0);
        StartX = flag->getWidth()+4;
    }
    snprintf(statBuf, sizeof(statBuf), "%-20s", state->getName().c_str());
    dst.bltString(StartX , 4, statBuf, ctTexteNormal);
}

void tPlayerStateBox::UpdateState(bool ForceUpdate)
{
    int nbPlayer = PlayerInterface::getActivePlayerCount();
    if (GameConfig::game_teammode && ShowTeam)
        nbPlayer = TeamManager::CountPlayerinTeam(TeamNumber);

    if ((nbPlayer != Count()) || ForceUpdate) 
    {
        Clear();
        for ( int i = 0; i < PlayerInterface::getMaxPlayers(); ++i)
        {
            PlayerState* state = PlayerInterface::getPlayer(i);
            if( state->isActive() )
            {
                if (ShowTeam)
                {
                    if (state->getTeamID() == TeamNumber)
                        AddData(state->getName(), state);
                } 
                else AddData(state->getName(), state);
            }
        }
    }
}
void tPlayerStateBox::setDrawFlags(bool df)
{
    DrawFlags = df;
    dirty = true;
}

void tPlayerStateBox::setShowTeam(Uint8 Team_Number)
{
    TeamNumber = Team_Number;
    ShowTeam = true;
    UpdateState(true);
}

