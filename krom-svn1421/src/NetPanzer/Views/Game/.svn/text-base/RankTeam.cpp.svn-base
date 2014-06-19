/*
Copyright (C) 2009 Netpanzer Team. (www.netpanzer.org), Laurent Jacques

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


#include "RankTeam.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/StrManager.hpp"
#include "Resources/ResourceManager.hpp"
#include "Views/Theme.hpp"

#define HEADER_HEIGHT 60
#define ENTRY_HEIGHT 18

#define TABLE_START (HEADER_HEIGHT + 2)

RankTeam::RankTeam() : GameTemplateView()
{
    setSearchName("RankTeam");
    setTitle(_("Round stats"));
    setSubTitle("");

    setAllowResize(false);
    setBordered(false);

    moveTo(GameConfig::interface_rankposition_x, GameConfig::interface_rankposition_y);
    resize(iXY(400, 200));
    checkArea(iXY(screen->getWidth(),screen->getHeight()));

    selected_line = -1;
    selected_col = -1;
    RectStates = getClientRect();
}

// doDraw
//---------------------------------------------------------------------------
void RankTeam::doDraw(Surface &viewArea, Surface &clientArea)
{
    unsigned int newheight = HEADER_HEIGHT
                             + (ENTRY_HEIGHT * (PlayerInterface::countPlayers()+1/2))
                             + 60;

    if ( newheight != (unsigned int)getSizeY() )
    {
        resize(400, newheight);
        RectStates = getClientRect();
        return; // this frame draws nothing
    }
    clientArea.BltRoundRect(RectStates, 14, Palette::darkbrown256.getColorArray());
    clientArea.RoundRect(RectStates, 14, ctWindowsBorder);

    drawTeamStats(clientArea);

    View::doDraw(viewArea, clientArea);
}

class StatesSortByTeam
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2)
    {
        if (state1->getTeamID() != state2->getTeamID())
            return state1->getTeamID() < state2->getTeamID();
        else
            return state1->getTotal() > state2->getTotal();
    }
};

static void DrawPanelUser(iRect rect, Surface& dest, const PlayerState* state)
{
    char statBuf[256];
    int x = rect.min.x+5;
    int y = rect.min.y+5;

    dest.bltString(x, y, state->getName().c_str(), ctTexteOver);
    snprintf(statBuf, sizeof(statBuf), "%5i %s", 
             state->getTotal(),
             _("Points"));
    dest.bltString(rect.max.x-100, y, statBuf, ctTexteOver);
    y += 10;
    dest.drawLine(x, y, rect.max.x-5, y, ctTexteOver);
    y += 5;
    snprintf(statBuf, sizeof(statBuf), "%s: %5i", 
             _("Frags"), 
             state->getKills());
    dest.bltString(x, y, statBuf, ctTexteNormal);
    y += 12;
    snprintf(statBuf, sizeof(statBuf), "%s: %5i", 
             _("deaths"), 
             state->getLosses());
    dest.bltString(x, y, statBuf, ctTexteNormal);
    x += 120;
    y = rect.min.y+20;
    snprintf(statBuf, sizeof(statBuf), "%s: %5i", 
             _("Objectives"),
             state->getObjectivesHeld());
    dest.bltString(x, y, statBuf, ctTexteNormal);
}

static void DrawTeamHeader(Surface& dest, const Uint8 team, int x, int y)
{
    char statBuf[256];
    int color = TeamManager::getTeamColor(team);
    int row = y;
    TeamManager::drawFlag( team, dest, x+10, row );
    snprintf(statBuf, sizeof(statBuf), "%s %s", 
             _("Team"),
             TeamManager::getTeamName(team).c_str());
    dest.bltString(x+30, row+7, statBuf, color);

    row+=25;
    snprintf(statBuf, sizeof(statBuf), "%12s: %8i", 
             _("Frags"),
             TeamManager::getKills(team));
    dest.bltString(x+10, row, statBuf, color);
    row+=15;
    snprintf(statBuf, sizeof(statBuf), "%12s: %8i", 
             _("deaths"),
             TeamManager::getLosses(team));
    dest.bltString(x+10, row, statBuf, color);
    row+=15;
    snprintf(statBuf, sizeof(statBuf), "%12s: %8i", 
             _("Objectives"),
             TeamManager::getObjectivesHeld(team));
    dest.bltString(x+10, row, statBuf, color);
}

void RankTeam::drawTeamStats(Surface& dest)
{
    unsigned int flagHeight = ResourceManager::getFlag(0)->getHeight();
    char statBuf[256];
    iRect r((RectStates.getSizeX()/2)-2, RectStates.min.y+5, (RectStates.getSizeX()/2)+2, RectStates.max.y-20);
    dest.fillRect(r, ctTexteDisable);

    states.clear();
    PlayerID i;
    for( i = 0; i < PlayerInterface::getMaxPlayers(); ++i)
    {
        PlayerState* state = PlayerInterface::getPlayer(i);
        if( state->isActive() )
        {
            states.push_back(state);
        }
    }
    std::sort(states.begin(), states.end(), StatesSortByTeam());

    DrawTeamHeader(dest, 0, 5, 7);
    DrawTeamHeader(dest, 1, (RectStates.getSizeX()/2)+5, 7);

    int cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
    int flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
    Surface * flag = 0;
    int cur_state = 1;

    dest.drawLine(RectStates.min.x+5, cur_line_pos+10, RectStates.max.x-5, cur_line_pos+10, ctTexteNormal);

    cur_line_pos += ENTRY_HEIGHT;
    flag_pos += ENTRY_HEIGHT;
    int current_Team = 0;
    int FLAG_START = 3+RectStates.min.x+5;
    int NAME_START = 3+RectStates.min.x+28;

    iRect infopanel(RectStates.min.x+10,RectStates.max.y-50, RectStates.max.x-10, RectStates.max.y-5);
    dest.fillRect(infopanel, ctWindowsbackground);

    for(std::vector<const PlayerState*>::iterator i = states.begin();
            i != states.end(); ++i)
    {
        const PlayerState* state = *i;

        if (current_Team != state->getTeamID())
        {
            cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
            flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
            cur_line_pos += ENTRY_HEIGHT;
            flag_pos += ENTRY_HEIGHT;
            current_Team = state->getTeamID();
            FLAG_START = (3+RectStates.getSizeX()/2)+5;
            NAME_START = (3+RectStates.getSizeX()/2)+28;
            cur_state = 1;
        }
        snprintf(statBuf, sizeof(statBuf),
                 "%-15s", state->getName().c_str());
        if (cur_state == selected_line)
        {
            if (selected_col == current_Team)
            {
                iRect r(FLAG_START, cur_line_pos-3, FLAG_START+185, cur_line_pos+11);
                dest.fillRect(r, ctTexteOver);
                DrawPanelUser(infopanel, dest, state);
            }
        }
        dest.bltString(NAME_START, cur_line_pos, statBuf, ctTexteNormal);

        flag = ResourceManager::getFlag(state->getFlag());
        flag->blt( dest, FLAG_START, flag_pos );

        cur_line_pos += ENTRY_HEIGHT;
        flag_pos += ENTRY_HEIGHT;
        ++cur_state;
    }

}

void RankTeam::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    GameTemplateView::mouseMove(prevPos, newPos);
    if ( newPos.y >= TABLE_START )
    {
        selected_line = (newPos.y-TABLE_START) / ENTRY_HEIGHT;
        if (newPos.x > RectStates.getSizeX()/2) selected_col = 1;
        else selected_col = 0;
    }
}

void RankTeam::notifyMoveTo()
{
    GameConfig::interface_rankposition_x = min.x;
    GameConfig::interface_rankposition_y = min.y;
}

void RankTeam::doActivate()
{
    selected_line = -1;
}

void RankTeam::doDeactivate()
{
    selected_line = -1;
}

void RankTeam::processEvents()
{
    COMMAND_PROCESSOR.process(false);
}
