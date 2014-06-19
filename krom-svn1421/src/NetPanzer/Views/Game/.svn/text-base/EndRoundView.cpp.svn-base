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


#include "EndRoundView.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Views/Components/Desktop.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Resources/ResourceManager.hpp"
#include "Views/Theme.hpp"

#define HEADER_HEIGHT 70
#define ENTRY_HEIGHT 20

#define TABLE_BORDER 2
#define TABLE_BORDER_START (HEADER_HEIGHT+TABLE_BORDER)
#define TABLE_START (HEADER_HEIGHT + TABLE_BORDER)

#define WINDOW_WIDTH (TABLE_HEADER_PIX_LEN + ((DEFAULT_BORDER_SIZE+TABLE_BORDER) * 2 ) + 14+2)

static const char * table_header =
    "          Name                 Frags Deaths Points Objs.";

static const char * stats_format = "%-20s%6i%7i%7i%6i";

EndRoundView::EndRoundView() : GameTemplateView()
{
    setSearchName("EndRoundView");
    setTitle("Round stats");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);
    setBordered(false);

    int x = (screen->getWidth() / 2) - 250;
    int y = (screen->getHeight() / 2) - 250;
    moveTo(iXY(x, y));
    resize(iXY(500, 500));
    checkArea(iXY(screen->getWidth(),screen->getHeight()));

    allyImage.loadBMP("pics/default/ally.bmp");
    allyRequestImage.loadBMP("pics/default/allyRequest.bmp");
    allyOtherImage.loadBMP("pics/default/allyOther.bmp");
    noAllyImage.loadBMP("pics/default/noAlly.bmp");
    colorImage.loadBMP("pics/default/playerColor.bmp");

    selected_line = -1;
    selected_col = -1;
    RectWinner = getClientRect();
    RectWinner.min.x = RectWinner.min.y = 0;
    RectStates = RectWinner;
    RectWinner.max.y = HEADER_HEIGHT-10;
    RectStates.min.y = HEADER_HEIGHT;

} // end EndRoundView::EndRoundView

// doDraw
//---------------------------------------------------------------------------
void EndRoundView::doDraw(Surface &viewArea, Surface &clientArea)
{
    unsigned int flagHeight = ResourceManager::getFlag(0)->getHeight();
    clientArea.BltRoundRect(RectWinner, 14, Palette::green256.getColorArray());
    clientArea.RoundRect(RectWinner,14, ctWindowsBorder);

    clientArea.BltRoundRect(RectStates, 14, Palette::darkbrown256.getColorArray());
    clientArea.RoundRect(RectStates, 14, ctWindowsBorder);

    if (GameConfig::game_teammode) drawTeamStats(clientArea, flagHeight);
    else drawPlayerStats(clientArea, flagHeight);

    View::doDraw(viewArea, clientArea);
} // end doDraw

class StatesSortByPoints
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2)
    {
        return state1->getTotal() > state2->getTotal();
    }
};

class StatesSortByObjectives
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2)
    {
        int p1 = state1->getObjectivesHeld();
        int p2 = state2->getObjectivesHeld();
        return p1 > p2 || (p1 == p2 && state1->getTotal() > state2->getTotal() );
    }
};


// drawPlayerStats
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void EndRoundView::drawPlayerStats(Surface &dest, unsigned int flagHeight)
{
    char statBuf[256];

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
    switch(GameConfig::game_gametype)
    {
    case _gametype_objective:
        std::sort(states.begin(), states.end(), StatesSortByObjectives());
        break;
    case _gametype_timelimit:
    case _gametype_fraglimit:
        std::sort(states.begin(), states.end(), StatesSortByPoints());
        break;
    }

    const PlayerState* Winner = states.front();
    snprintf(statBuf, sizeof(statBuf),
             "Winner is %s", Winner->getName().substr(0,20).c_str());
    dest.bltStringCenteredInRect(RectWinner,statBuf, Color::yellow);


    int cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
    int flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
    Surface * flag = 0;
    int cur_state = 0;

    dest.bltString(0, cur_line_pos, table_header, ctTexteNormal);
    cur_line_pos += ENTRY_HEIGHT;
    flag_pos += ENTRY_HEIGHT;
    ++cur_state;
    int ALLY_START = RectStates.min.x+4;
    int COLOR_START = ALLY_START+20;
    int FLAG_START = COLOR_START+15;
    int NAME_START = FLAG_START+25;

    
    for(std::vector<const PlayerState*>::iterator i = states.begin();
            i != states.end(); ++i)
    {
        const PlayerState* state = *i;

        snprintf(statBuf, sizeof(statBuf),
                 stats_format, state->getName().substr(0,20).c_str(),
                 state->getKills(), state->getLosses(), state->getTotal(),
                 state->getObjectivesHeld());
        dest.bltString(NAME_START, cur_line_pos, statBuf,
                       (cur_state == selected_line)?Color::yellow:ctTexteNormal);

        flag = ResourceManager::getFlag(state->getFlag());
        flag->blt( dest, FLAG_START, flag_pos );
        if ( state->getID() != PlayerInterface::getLocalPlayerIndex() )
        {
            // XXX ALLY
            bool meWithHim = PlayerInterface::isSingleAllied(PlayerInterface::getLocalPlayerIndex(), state->getID());
            bool himWithMe = PlayerInterface::isSingleAllied(state->getID(), PlayerInterface::getLocalPlayerIndex());
            if ( meWithHim && himWithMe )
            {
                allyImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else if ( meWithHim )
            {
                allyRequestImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else if ( himWithMe )
            {
                allyOtherImage.bltTrans(dest, ALLY_START, flag_pos );
            }
            else
            {
                noAllyImage.bltTrans(dest, ALLY_START, flag_pos );
            }
        }

        colorImage.bltTransColor(dest, COLOR_START, flag_pos, state->getColor());

        cur_line_pos += ENTRY_HEIGHT;
        flag_pos += ENTRY_HEIGHT;
        ++cur_state;
    }

}

class StatesSortByTeam
    : public std::binary_function<const PlayerState*, const PlayerState*, bool>
{
public:
    bool operator() (const PlayerState* state1, const PlayerState* state2)
    {
        return state1->getTeamID() < state2->getTeamID();
    }
};

static void DrawPanelUser(iRect rect, Surface& dest, const PlayerState* state)
{
    char statBuf[256];
    int x = rect.min.x+5;
    int y = rect.min.y+5;
    
    dest.bltString(x, y, state->getName().c_str(), ctTexteOver);
    snprintf(statBuf, sizeof(statBuf), "%5i Points", state->getTotal());
    dest.bltString(rect.max.x-100, y, statBuf, ctTexteOver);
    y += 10;
    dest.drawLine(x, y, rect.max.x-5, y, ctTexteOver);
    y += 5;
    snprintf(statBuf, sizeof(statBuf), " Frags: %5i", state->getKills());
    dest.bltString(x, y, statBuf, ctTexteNormal);
    y += 12;
    snprintf(statBuf, sizeof(statBuf), "deaths: %5i", state->getLosses());
    dest.bltString(x, y, statBuf, ctTexteNormal);
    x += 120;
    y = rect.min.y+20;
    snprintf(statBuf, sizeof(statBuf), "Objectives: %5i", state->getObjectivesHeld());
    dest.bltString(x, y, statBuf, ctTexteNormal);
}

void EndRoundView::drawTeamStats(Surface& dest, unsigned int flagHeight)
{
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

    Uint8 TeamWin = TeamManager::getTeamWin();

    TeamManager::drawFlag(TeamWin, dest, RectWinner.min.x+(RectWinner.getSizeX()/2)-7, RectWinner.min.y+5 );

    snprintf(statBuf, sizeof(statBuf),
             "Winner is Team %s", TeamManager::getTeamName(TeamWin).substr(0,20).c_str());
    dest.bltStringCenteredInRect(RectWinner,statBuf, Color::yellow);

    snprintf(statBuf, sizeof(statBuf),
             "frags: %5i  deaths: %5i  Objectives: %5i", 
             TeamManager::getKills(TeamWin),
             TeamManager::getLosses(TeamWin),
             TeamManager::getObjectivesHeld(TeamWin));
    dest.bltString(RectWinner.min.x+50, RectWinner.max.y-15,statBuf, ctTexteNormal);

    int cur_line_pos = TABLE_START + ((ENTRY_HEIGHT - Surface::getFontHeight())/2);
    int flag_pos = TABLE_START + (int(ENTRY_HEIGHT - flagHeight))/2;
    Surface * flag = 0;
    int cur_state = 1;

    dest.bltString(0, cur_line_pos, "      Name             Points", ctTexteNormal);
    dest.bltString(5+RectStates.getSizeX()/2, cur_line_pos, "      Name             Points", ctTexteNormal);
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
                 "%-15s%7i", state->getName().c_str(), state->getTotal());
        if (cur_state == selected_line)
        {
            if (selected_col == current_Team)
            {
                iRect r(FLAG_START, cur_line_pos-3, FLAG_START+235, cur_line_pos+11);
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

void EndRoundView::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    GameTemplateView::mouseMove(prevPos, newPos);
//    selected_line = -1;
//    selected_col = -1;
    if ( newPos.y >= TABLE_START )
    {
        selected_line = (newPos.y-TABLE_START) / ENTRY_HEIGHT;
        if (newPos.x > RectStates.getSizeX()/2) selected_col = 1;
        else selected_col = 0;
    }
}

void EndRoundView::doActivate()
{
    selected_line = -1;
}

void EndRoundView::doDeactivate()
{
    selected_line = -1;
}

void EndRoundView::checkResolution(iXY oldResolution, iXY newResolution)
{
    int x = (screen->getWidth() / 2) - 250;
    int y = (screen->getHeight() / 2) - 250;
    moveTo(iXY(x, y));
}
