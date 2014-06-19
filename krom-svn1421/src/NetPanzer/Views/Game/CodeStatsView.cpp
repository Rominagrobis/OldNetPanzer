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


#include "CodeStatsView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Classes/SpriteSorter.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/AI/PathingState.hpp"
#include "Interfaces/PathScheduler.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Classes/Network/NetworkInterface.hpp"
#include "Actions/Action.hpp"
#include "Views/Components/Button.hpp"

enum{ _display_mode_network_stats,
      _display_mode_pathing_stats,
      _display_mode_sorter_stats,
      _display_mode_unit_stats
    };

int gPacketSize = 0;
int display_mode;

class SetDisplayModeAction : public Action
{
public:
    int mode;
    SetDisplayModeAction(int mode) : Action(false), mode(mode) {}
    void execute()
    {
        display_mode = mode;
    }
};

class UpdateNetworkStatsAction : public Action
{
public:
    UpdateNetworkStatsAction() : Action(false) {}
    void execute()
    {
        NetworkState::logNetworkStats();
    }
};

static long INFO_AREA_Y_OFFSET = 60;

CodeStatsView::CodeStatsView() : GameTemplateView()
{
    setSearchName("CodeStatsView");
    setTitle("CodeStatsView");
    setSubTitle(" - F4");

    setAllowResize(false);
    setAllowMove(true);
    setDisplayStatusBar(true);
    setVisible(false);

    moveTo(iXY(0, 0));

    iXY  area_size = iXY(280, 280) ;
    long bXOffset;
    INFO_AREA_Y_OFFSET = 2;

    resizeClientArea(area_size);

    bXOffset = area_size.x / 3;
    add( Button::createTextButton("Net",    iXY(0, INFO_AREA_Y_OFFSET),              bXOffset-3,    new SetDisplayModeAction(_display_mode_network_stats)));
    add( Button::createTextButton("Sprite", iXY(bXOffset+1, INFO_AREA_Y_OFFSET),     bXOffset-4,    new SetDisplayModeAction(_display_mode_sorter_stats)));
    add( Button::createTextButton("Unit",   iXY((bXOffset*2)+1, INFO_AREA_Y_OFFSET), bXOffset-2,    new SetDisplayModeAction(_display_mode_unit_stats)));

    INFO_AREA_Y_OFFSET += 18;
    add( Button::createTextButton("Path",   iXY(0, INFO_AREA_Y_OFFSET),              area_size.x-2, new SetDisplayModeAction(_display_mode_pathing_stats)));

    INFO_AREA_Y_OFFSET += 18;
    bXOffset = area_size.x / 3;
    add( Button::createTextButton("NetLog", iXY((bXOffset*2)+1, INFO_AREA_Y_OFFSET), bXOffset-2,    new UpdateNetworkStatsAction()));

    INFO_AREA_Y_OFFSET += 18;

    display_mode = _display_mode_network_stats;

} // end CodeStatsView::CodeStatsView

// doDraw
//---------------------------------------------------------------------------
void CodeStatsView::doDraw(Surface &viewArea, Surface &clientArea)
{
    bltViewBackground(viewArea);

    switch( display_mode ) {
    case _display_mode_network_stats :
        drawNetworkStats( clientArea );
        break;

    case _display_mode_sorter_stats :
        drawSorterStats( clientArea );
        break;

    case _display_mode_pathing_stats :
        drawPathingStats( clientArea );
        break;

    case _display_mode_unit_stats :
        drawUnitStats( clientArea );
        break;
    }

    View::doDraw(viewArea, clientArea);
} // end CodeStatsView::doDraw

//---------------------------------------------------------------------------
void CodeStatsView::drawNetworkStats(Surface &clientArea)
{

    char strBuf[256];

    iXY str_loc(2, INFO_AREA_Y_OFFSET);

    sprintf(strBuf, "Packets" );

    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Sent %ld, %.4f 1/s, %.4f Avg",
                    NetworkState::packets_sent,
                    NetworkState::packets_sent_per_sec,
                    ((float)NetworkState::packets_sent) / ((float)NetworkState::packets_sent_time) );

    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Recv %ld, %.4f 1/s, %.4f Avg",
                    NetworkState::packets_received,
                    NetworkState::packets_received_per_sec,
                    ((float)NetworkState::packets_received) / ((float)NetworkState::packets_received_time) );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Bytes" );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Sent %ld, %.4f 1/s, %.4f Avg",
                    NetworkState::bytes_sent,
                    NetworkState::bytes_sent_per_sec,
                    ((float) NetworkState::bytes_sent) / ((float) NetworkState::packets_sent_time) );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Recv %ld, %.4f 1/s, %.4f Avg",
                    NetworkState::bytes_received,
                    NetworkState::bytes_received_per_sec,
                    ((float) NetworkState::bytes_received) / ((float) NetworkState::packets_received_time) );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Opcodes" );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Sent : %ld, %.4f 1/s ",
                    NetworkState::opcodes_sent,
                    NetworkState::opcodes_sent_per_sec );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Recv : %ld, %.4f 1/s",
                    NetworkState::opcodes_received,
                    NetworkState::opcodes_received_per_sec );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    if ( NetworkState::status == _network_state_client ) {
        sprintf(strBuf, "Ping Time: %.4f (ms)", NetworkState::ping_time );
    } else {
        sprintf(strBuf, "Ping Time: NA" );
    }

    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);
    str_loc.y += 12;
}

//---------------------------------------------------------------------------
void CodeStatsView::drawSorterStats(Surface &clientArea)
{

    char strBuf[256];

    iXY str_loc(2, INFO_AREA_Y_OFFSET);

    sprintf(strBuf, "Sorter" );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    for ( unsigned long i = 0; i < _MAX_HEIGHT_LEVELS; i++ ) {
        sprintf(strBuf, "Level %lu : %lu  ", i, SPRITE_SORTER.getMaxSprites( i ) );
        clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);
        str_loc.y += 12;
    }

}

//---------------------------------------------------------------------------
void CodeStatsView::drawPathingStats(Surface &clientArea)
{

    char strBuf[256];

    iXY str_loc(2, INFO_AREA_Y_OFFSET);

    sprintf(strBuf, "Pathing" );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "A* Time: %.6f s", PathingState::astar_gen_time );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "A* Avg: %.6f s", PathingState::getAverageAstarPathTime() );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "A* Total: %.6f s", PathingState::astar_gen_time_total );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);


    str_loc.y += 12;

    sprintf(strBuf, "Path Length: %ld ", PathingState::path_length );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);


    str_loc.y += 12;

    sprintf(strBuf, "Paths: %lu ", PathingState::path_gen_count );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Updates: %lu ", PathingState::update_gen_count );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Cache Hits: %lu ", PathingState::path_cache_hits );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    sprintf(strBuf, "Cache Misses: %lu ", PathingState::path_cache_misses );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);
}

//---------------------------------------------------------------------------
void CodeStatsView::drawUnitStats(Surface &clientArea)
{

    char strBuf[256];
    int total = 0;
    PlayerID max_players;


    iXY str_loc(2, INFO_AREA_Y_OFFSET);

    sprintf(strBuf, "Units" );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

    max_players = PlayerInterface::getMaxPlayers();
    PlayerID i;
    for ( i = 0; i < max_players; ++i )
    {
        unsigned long units;
        units = UnitInterface::getUnitCount( i );
        total += units;

        sprintf(strBuf, "Player %d : %lu  ", i, units);
        clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);
        str_loc.y += 12;
    }

    sprintf(strBuf, "UnitTotal : %d", total );
    clientArea.bltString(str_loc.x, str_loc.y, strBuf, Color::white);

    str_loc.y += 12;

}
