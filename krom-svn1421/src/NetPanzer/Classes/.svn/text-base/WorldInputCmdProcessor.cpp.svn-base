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

#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>
#include "WorldInputCmdProcessor.hpp"

#include "Interfaces/MouseInterface.hpp"
#include "Interfaces/KeyboardInterface.hpp"

#include "Interfaces/WorldViewInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/VoteManager.hpp"

#include "Units/UnitInterface.hpp"

#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"

#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetMessageEncoder.hpp"
#include "Classes/PlacementMatrix.hpp"
#include "System/Sound.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Util/Log.hpp"
#include "Util/NTimer.hpp"

#include "Interfaces/GameConfig.hpp"

#include "Views/Game/VehicleSelectionView.hpp"
#include "Views/Components/Desktop.hpp"

#include "Classes/Network/NetworkState.hpp"

#include "Interfaces/ConsoleInterface.hpp"

#include "Units/Vehicle.hpp"

#include "Scripts/ScriptManager.hpp"
#include "Views/Game/ChatView.hpp"
#include "Actions/ActionManager.hpp"

#include "Network/PlayerRequests/AttackUnitRequest.hpp"
#include "Network/PlayerRequests/ManualFireRequest.hpp"
#include "Network/PlayerRequests/MoveUnitRequest.hpp"
#include "Network/PlayerRequests/ChangeObjectiveOutLocationRequest.hpp"
#include "Network/PlayerRequests/AllianceRequest.hpp"
#include "Network/PlayerRequests/BreakAllianceRequest.hpp"

WorldInputCmdProcessor COMMAND_PROCESSOR;

enum { _cursor_regular,
       _cursor_move,
       _cursor_blocked,
       _cursor_player_unit,
       _cursor_enemy_unit,
       _cursor_blocked_target,
       _cursor_make_allie,
       _cursor_break_allie
     };

ObjectiveID WorldInputCmdProcessor::selected_objective_id = 0;

WorldInputCmdProcessor::WorldInputCmdProcessor()
{
    selection_box_active = false;
    outpost_goal_selection = OBJECTIVE_NONE;
    previous_manual_control_state = false;
    manual_control_state = false;
    manual_fire_state = false;

    right_mouse_scroll = false;
    right_mouse_scroll_moved = false;

    lastSelectTimer.setTimeOut(400);
    actionTimer.setTimeOut(100);
    Flagtimer.reset();

    last_selected_group = -1;
}

void
WorldInputCmdProcessor::switchSelectionList(unsigned long new_list_index)
{
    working_list.copyList( selection_group_lists[ new_list_index ] );
    working_list.select();
    last_selected_group = new_list_index;
}

void WorldInputCmdProcessor::setSelectionList(unsigned long new_list_index)
{
    selection_group_lists[new_list_index].copyList(working_list);
    working_list.select();
    last_selected_group = new_list_index;
}

void
WorldInputCmdProcessor::cycleSelectedUnits(unsigned long new_list_index)
{
    if (current_selection_list_index != new_list_index) {
        working_list.copyList(selection_group_lists[new_list_index]);
        current_selection_list_index = new_list_index;
    }

    working_list.cycleNextUnit();
}

void
WorldInputCmdProcessor::updateScrollStatus(const iXY &mouse_pos)
{
    double time_slice;
    long  scroll_increment;
    float scroll_rate;

    time_slice = TimerInterface::getTimeSlice();
    scroll_rate = GameConfig::interface_scrollrate;

    scroll_increment = (long) (scroll_rate * time_slice);

    if(right_mouse_scroll)
    {
        int x,y;
        int buttons=SDL_GetMouseState(&x,&y);
        if(!(buttons&SDL_BUTTON(SDL_BUTTON_RIGHT)))
        {
            // sometimes the winning page or something comes up
            //  as you're holding down the right mouse button
            //  and the UP message doesn't come through
            right_mouse_scroll=false;
        }
        else if(mouse_pos.x!=right_mouse_scroll_pos.x || mouse_pos.y!=right_mouse_scroll_pos.y)
        {
            // we're holding down the right mouse button, and mouse has moved
            int x_move=mouse_pos.x-right_mouse_scroll_pos.x;
            int y_move=mouse_pos.y-right_mouse_scroll_pos.y;
            SDL_WarpMouse(right_mouse_scroll_pos.x,right_mouse_scroll_pos.y);

            WorldViewInterface::scroll_right(x_move*4);
            WorldViewInterface::scroll_down(y_move*4);
            right_mouse_scrolled_pos.x+=x_move;
            right_mouse_scrolled_pos.y+=y_move;
            right_mouse_scroll_moved = true;
        }
        return;
    }

    if ( GameConfig::video_fullscreen != true ) {
        // don't do border scrolling on windowed mode because
        //  the window isn't always on the edge of the screen.
        return;
    }

    if((unsigned int)mouse_pos.x >= (screen->getWidth() - 1)) {
        WorldViewInterface::scroll_right(scroll_increment);
    }

    if( mouse_pos.x < 1) {
        WorldViewInterface::scroll_left(scroll_increment);
    }

    if( (unsigned int)mouse_pos.y >= (screen->getHeight() - 1)) {
        WorldViewInterface::scroll_down(scroll_increment);
    }

    if( mouse_pos.y < 1) {
        WorldViewInterface::scroll_up(scroll_increment);
    }
}


unsigned char
WorldInputCmdProcessor::getCursorStatus(const iXY& loc)
{
    iXY map_loc;
    bool fielddraws = false;

    if( ((manual_control_state == true) || (manual_fire_state == true)) && working_list.isSelected() )
    {
        unsigned long id_list_index;
        size_t id_list_size;
        UnitBase *unit_ptr;

        id_list_size = working_list.unit_list.size();
        for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ )
        {
            unit_ptr = UnitInterface::getUnit(working_list.unit_list[ id_list_index ]);
            if ( unit_ptr && unit_ptr->isWeaponInRange(loc) )
            {
                fielddraws = true;
                return  _cursor_enemy_unit;
            }
        }
    }

    if ((fielddraws == false) && ((manual_control_state == true) || (manual_fire_state == true)) && working_list.isSelected())
    {
        return _cursor_blocked_target;
    }
    MapInterface::pointXYtoMapXY( loc, map_loc );

    if (MapInterface::getMovementValue(map_loc) >= 0xFF)
    {
        return _cursor_blocked;
    }

    UnitID unit_id;
    if ( UnitInterface::queryUnitAtMapLoc(map_loc, &unit_id) )
    {
        UnitBase * unit = UnitInterface::getUnit(unit_id);
        if ( unit->player_id == PlayerInterface::getLocalPlayerIndex() )
        {
            return _cursor_player_unit;
        }
        // XXX ALLY
        if ( ! PlayerInterface::isAllied(unit->player_id, PlayerInterface::getLocalPlayerIndex() ) )
        {
            if ( KeyboardInterface::getKeyState(SDLK_a) && !GameConfig::game_teammode )
            {
                if ( PlayerInterface::isSingleAllied(PlayerInterface::getLocalPlayerIndex(), unit->player_id ) )
                {
                    return _cursor_break_allie;
                }

                return _cursor_make_allie;
            }
            else if ( working_list.isSelected() && (fielddraws == true))
            {
                return _cursor_enemy_unit;
            }
            else if ( working_list.isSelected() && (fielddraws == false) && (manual_fire_state == false))
            {
                return _cursor_enemy_unit;
            }
        }
        else
        {
            if ( KeyboardInterface::getKeyState(SDLK_a) && !GameConfig::game_teammode )
            {
                return _cursor_break_allie;
            }
        }
        //return _cursor_regular;
    }

    if (working_list.isSelected())
        return _cursor_move;

    if (selection_box_active)
        return _cursor_regular;

    return _cursor_regular;
}

void
WorldInputCmdProcessor::setMouseCursor(unsigned char world_cursor_status)
{
    switch(world_cursor_status)
    {
        case _cursor_regular :
            MouseInterface::setCursor("default.bmp");
            break;

        case _cursor_move :
            MouseInterface::setCursor("move.bmp");
            break;

        case _cursor_blocked :
            MouseInterface::setCursor("noentry.bmp");
            break;

        case _cursor_player_unit :
            MouseInterface::setCursor("select.bmp");
            break;

        case _cursor_enemy_unit :
            MouseInterface::setCursor("target.bmp");
            break;

        case _cursor_make_allie :
            MouseInterface::setCursor("allie.bmp");
            break;

        case _cursor_break_allie :
            MouseInterface::setCursor("breakallie.bmp");
            break;
        case _cursor_blocked_target :
            MouseInterface::setCursor("grey-target.bmp");
            break;
    }
}

void
WorldInputCmdProcessor::getManualControlStatus()
{
    if ( KeyboardInterface::getKeyState( SDLK_LCTRL ) ||
            KeyboardInterface::getKeyState( SDLK_RCTRL )
       )
    {
        manual_fire_state = true;
    }
    else
    {
        manual_fire_state = false;
    }
}

void
WorldInputCmdProcessor::evaluateKeyCommands()
{
    if (KeyboardInterface::getKeyPressed(SDLK_b)
       && ! Desktop::getVisible("HelpScrollView") )
    {
        if ( PlayerInterface::getLocalPlayer()->isSelectingFlag()
            || GameConfig::game_changeflagtime == 0
            || Flagtimer.checkWithTimeOut(GameConfig::game_changeflagtime * 60000)
           )
        {
            Desktop::toggleVisibility( "GFlagSelectionView" );
        }
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_m) )
    {
        Desktop::toggleVisibility( "MiniMapView" );
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_o) )
    {
        toggleDisplayOutpostNames();
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_f) )
    {
        GameConfig::interface_show_flags = !GameConfig::interface_show_flags;
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_v) )
    {
        ActionManager::runAction("startvote_surrender");
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_n) )
    {
        GameConfig::interface_show_names = !GameConfig::interface_show_names;
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_d) )
    {
        GameConfig::interface_show_health = !GameConfig::interface_show_health;
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_c) )
    {
        static NTimer spamtimer(5000);
        if (spamtimer.isTimeOut())
        {
            ScriptManager::runUserCommand("countdown 5 Prepare to fight...");
            spamtimer.reset();
        }
    }

    if ( KeyboardInterface::getKeyPressed(SDLK_F1)
       && ! Desktop::getVisible("GFlagSelectionView") ) {
        Desktop::toggleVisibility( "HelpScrollView" );
    }

    if (KeyboardInterface::getKeyPressed(SDLK_TAB) )
    {
        if (GameConfig::game_teammode)
            Desktop::toggleVisibility( "RankTeam" );
        else
            Desktop::toggleVisibility( "RankView" );
    }

    if (KeyboardInterface::getKeyPressed(SDLK_F3)) {
        Desktop::toggleVisibility( "DesktopView" );
    }

    if (KeyboardInterface::getKeyPressed(SDLK_F4)) {
        Desktop::toggleVisibility( "CodeStatsView" );
    }

    if (KeyboardInterface::getKeyPressed( SDLK_F5 )) {
        //  DEBUG VIEW
        Desktop::toggleVisibility( "LibView" );
    }

    if ( (KeyboardInterface::getKeyPressed( SDLK_RETURN ) == true)
            && (KeyboardInterface::getKeyState( SDLK_LALT ) == false)
            && (KeyboardInterface::getKeyState( SDLK_RALT ) == false))
    {
        ChatView *v = (ChatView*)Desktop::getView("ChatView");
        if ( v )
        {
            v->openChat();
        }
    }

    if ( (KeyboardInterface::getKeyState(SDLK_LCTRL) || KeyboardInterface::getKeyState(SDLK_RCTRL))
        && (KeyboardInterface::getKeyPressed(SDLK_RETURN) ))
    {
        ChatView *v = (ChatView*)Desktop::getView("ChatView");
        if ( v )
        {
            v->openFriendsChat();
        }
    }

    //If space is pressed, jump to first currently attacked unit
    if( KeyboardInterface::getKeyPressed( SDLK_SPACE ) == true )
    {
        jumpLastAttackedUnit();
    }

    //If any of the four arrow-keys is pressed, scroll
    if( KeyboardInterface::getKeyState( SDLK_UP ) == true )
    {
    	WorldViewInterface::scroll_up( 15 );
    }
    else if( KeyboardInterface::getKeyState( SDLK_DOWN ) == true )
    {
    	WorldViewInterface::scroll_down( 15 );
    }

    if( KeyboardInterface::getKeyState( SDLK_RIGHT ) == true )
    {
    	WorldViewInterface::scroll_right( 15 );
    }
    else if( KeyboardInterface::getKeyState( SDLK_LEFT ) == true )
    {
    	WorldViewInterface::scroll_left( 15 );
    }

    if ( KeyboardInterface::getKeyPressed( SDLK_PAGEUP ) )
    {
        ActionManager::runAction("chat_pgup");
    }

    if ( KeyboardInterface::getKeyPressed( SDLK_PAGEDOWN ) )
    {
        ActionManager::runAction("chat_pgdown");
    }


}

void
WorldInputCmdProcessor::jumpLastAttackedUnit()
{
    const UnitInterface::Units& units = UnitInterface::getUnits();
    for(UnitInterface::Units::const_iterator i = units.begin();
            i != units.end(); ++i)
    {
        UnitBase* unit = i->second;
        if(unit->player_id != PlayerInterface::getLocalPlayerIndex())
            continue;

        if (unit->unit_state.threat_level == _threat_level_under_attack)
        {
            WorldViewInterface::setCameraPosition(unit->unit_state.location);
	    break;
        }
    }
}

void
WorldInputCmdProcessor::evaluateGroupingKeys()
{
    int group = -1;

    for (int k = SDLK_0; k <= SDLK_9; ++k )
    {
        if ( KeyboardInterface::getKeyPressed(k) )
        {
            group = k-SDLK_0;
            break;
        }
    }

    if ( group == -1 ) return;

    bool alt_status = KeyboardInterface::getKeyState( SDLK_LALT ) || KeyboardInterface::getKeyState( SDLK_RALT);
    bool ctrl_status = KeyboardInterface::getKeyState(SDLK_LCTRL) || KeyboardInterface::getKeyState(SDLK_RCTRL);

    if ( ctrl_status && alt_status ) return; // nothing to do

    if ( ctrl_status )
    {
        setSelectionList(group);
        ConsoleInterface::postMessage(Color::brown, false, 0, "Group %d created", group ? group : 10);
        return;
    }

    if ( alt_status )
    {
        cycleSelectedUnits(group);
        return;
    }

    // if shift pressed will keep the selected units.
    if ( !KeyboardInterface::getKeyState(SDLK_LSHIFT) && !KeyboardInterface::getKeyState(SDLK_RSHIFT) )
    {
        working_list.unGroup();
    }

    working_list.addList( selection_group_lists[group] );
    working_list.select();

    if ( last_selected_group == group && !lastSelectTimer.isTimeOut() )
    {
        centerSelectedUnits();
    }
    lastSelectTimer.reset();
    last_selected_group = group;

}

void
WorldInputCmdProcessor::evaluateKeyboardEvents()
{
    getManualControlStatus();

    evaluateGroupingKeys();

    evaluateKeyCommands();

    previous_manual_control_state = manual_control_state;
}

bool
WorldInputCmdProcessor::selectBoundBoxUnits()
{
    bool select_success;
    iRect r;

    if ( box_press.x > box_release.x ) {
        r.min.x=box_release.x;
        r.max.x=box_press.x;
    } else {
        r.min.x=box_press.x;
        r.max.x=box_release.x;
    }

    if ( box_press.y > box_release.y ) {
        r.min.y=box_release.y;
        r.max.y=box_press.y;
    } else {
        r.min.y=box_press.y;
        r.max.y=box_release.y;
    }

    bool addunits = false;
    if(KeyboardInterface::getKeyState(SDLK_LSHIFT) ||
            KeyboardInterface::getKeyState(SDLK_RSHIFT))
        addunits = true;

    select_success = working_list.selectBounded(r, addunits);

    if ( select_success == false ) {
        iXY box_size;
        box_size = box_release - box_press;
        if ( (box_size.x > 40) || (box_size.y > 40) )
            select_success = true;

        return( select_success );
    } else {
        current_selection_list_index = 0xFFFF;
        return( select_success );
    }
}

void
WorldInputCmdProcessor::evaluateMouseEvents()
{
    iXY world_pos;
    iXY mouse_pos;

    MouseInterface::getMousePosition( &mouse_pos.x, &mouse_pos.y );
    WorldViewInterface::getViewWindow( &world_win );

    WorldViewInterface::clientXYtoWorldXY( world_win, mouse_pos, &world_pos );
    setMouseCursor(getCursorStatus(world_pos));

    if(selection_box_active) {
        box_release = world_pos;
        if(abs(box_release.x - box_press.x) > 3
                    && abs(box_release.y - box_press.y) > 3) {
            selectBoundBoxUnits();
        }
    }

    while( !MouseInterface::event_queue.empty() ) {
        MouseEvent event = MouseInterface::event_queue.front();
        MouseInterface::event_queue.pop_front();

        if( event.button == MouseInterface::left_button )
            evalLeftMButtonEvents(event);

        if( event.button == MouseInterface::right_button )
            evalRightMButtonEvents(event);
    }
}

void WorldInputCmdProcessor::evalLeftMButtonDownEvents(const MouseEvent &event)
{
    iXY world_pos;
    WorldViewInterface::clientXYtoWorldXY(world_win, event.pos, &world_pos);

    if ( (manual_control_state == true) ||
          KeyboardInterface::getKeyState( SDLK_LCTRL ) ||
          KeyboardInterface::getKeyState( SDLK_RCTRL ))
    {
        sendManualFireCommand( world_pos );
        return;
    }
    Objective *objective = ObjectiveInterface::getObjectiveAtWorldXY(world_pos);

    if ( objective && objective->occupying_player == PlayerInterface::getLocalPlayer() )
    {
        selection_box_active = false;
        outpost_goal_selection = objective->id;
        output_pos_press = objective->location;
    }
    else
    {
        box_press = world_pos;
        box_release = world_pos;
        selection_box_active = true;
    }

    unsigned char click_status;

    click_status = getCursorStatus(world_pos);
    switch(click_status)
    {
        case _cursor_enemy_unit:
            sendAttackCommand(world_pos);
            break;

        case _cursor_make_allie:
            sendAllianceRequest(world_pos, true);
            break;

        case _cursor_break_allie:
            sendAllianceRequest(world_pos, false);
            break;
    }
}
void WorldInputCmdProcessor::evalLeftMButtonUpEvents(const MouseEvent &event)
{
    iXY world_pos;
    unsigned char click_status;

    WorldViewInterface::clientXYtoWorldXY(world_win, event.pos, &world_pos);
    if (selection_box_active)
    {
        selection_box_active = false;
        box_release = world_pos;
        if(abs(box_release.x - box_press.x) > 3
        && abs(box_release.y - box_press.y) > 3)
        {
            return;
        }
    }

    if (outpost_goal_selection != OBJECTIVE_NONE )
    {
        Objective *objective = ObjectiveInterface::getObjectiveAtWorldXY(world_pos);

        if ( objective
             && objective->occupying_player == PlayerInterface::getLocalPlayer()
             && outpost_goal_selection == objective->id )
        {
            // we've let go of the mouse on the building so we're
            //  not changing the spawn point
            selected_objective_id = objective->id;
            activateVehicleSelectionView( selected_objective_id );
        }
        else
        {
            ChangeObjectiveOutLocationRequest msg;
            msg.setObjectiveId(outpost_goal_selection);
            msg.setMapPos(iXY(MapInterface::pointXtoMapX(world_pos.x), MapInterface::pointYtoMapY(world_pos.y)));
            CLIENT->sendMessage(&msg, sizeof(msg));
        }
        outpost_goal_selection = OBJECTIVE_NONE;
        return;
    }

    click_status = getCursorStatus(world_pos);
    switch(click_status)
    {
        case _cursor_player_unit:
        {
            static NTimer dclick_timer(200);
            static int click_times = 0;
            bool addunits = false;
            if( (KeyboardInterface::getKeyState(SDLK_LSHIFT) == true) ||
                    (KeyboardInterface::getKeyState(SDLK_RSHIFT) == true))
            {
                addunits = true;
            }
            if ( ! dclick_timer.isTimeOut() )
            {
                if ( click_times )
                {
                    iRect wr;
                    WorldViewInterface::getViewWindow(&wr);
                    working_list.selectBounded(wr, addunits);
                    click_times=0;
                }
                else
                {
                    working_list.selectSameTypeVisible(world_pos,addunits);
                    dclick_timer.reset();
                    click_times++;
                }
                break;
            }
            else if (addunits)
            {
                working_list.addUnit(world_pos);
            }
            else
            {
                working_list.selectUnit(world_pos );
            }

            current_selection_list_index = 0xFFFF;
            if (working_list.unit_list.size() > 0)
            {
                UnitBase *unit = UnitInterface::getUnit(
                        working_list.unit_list[0]);
                if(unit)
                    unit->soundSelected();
            }
            dclick_timer.reset();
            click_times=0;
            break;
        }
        case _cursor_move:
            if(outpost_goal_selection == OBJECTIVE_NONE)
                sendMoveCommand(world_pos);
            break;

    }
}
void
WorldInputCmdProcessor::evalLeftMButtonEvents(const MouseEvent &event)
{
    if(event.event == MouseEvent::EVENT_DOWN) evalLeftMButtonDownEvents(event);
    else if(event.event == MouseEvent::EVENT_UP) evalLeftMButtonUpEvents(event);
}

void WorldInputCmdProcessor::evalRightMButtonEvents(const MouseEvent& event)
{
    static NTimer mtimer(75);
    if (event.event == MouseEvent::EVENT_DOWN )
    {
        right_mouse_scroll=true;
        right_mouse_scroll_moved = false;
        right_mouse_scroll_pos=event.pos;
        right_mouse_scrolled_pos.x=right_mouse_scrolled_pos.y=0;
        mtimer.reset();
    }

    if (right_mouse_scroll && event.event == MouseEvent::EVENT_UP )
    {
        right_mouse_scroll=false;
        if ( ! right_mouse_scroll_moved && mtimer.isTimeOut() )
        {
            // simple right click on the same position after timeout
            working_list.unGroup();
        }
        return;
    }
}

void
WorldInputCmdProcessor::sendMoveCommand(const iXY& world_pos)
{
    if ( !actionTimer.isTimeOut() )
    {
        return;
    }

    actionTimer.reset();

    iXY map_pos;
    PlacementMatrix matrix;

    unsigned long id_list_index;
    size_t id_list_size;
    UnitBase *unit_ptr;

    MoveUnitRequest req;
    
    id_list_size = working_list.unit_list.size();

    if ( id_list_size == 0 )
        return;

    MapInterface::pointXYtoMapXY( world_pos, map_pos );
    matrix.reset( map_pos );

    NetMessageEncoder encoder;

    for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ )
    {
        unit_ptr = UnitInterface::getUnit(working_list.unit_list[ id_list_index ]);

        if ( unit_ptr != 0 )
        {
            if ( unit_ptr->unit_state.select == true )
            {
                matrix.getNextEmptyLoc( &map_pos );
                
                req.setUnitId(unit_ptr->id);
                req.setMapPos(map_pos);
                
                if ( !encoder.encodeMessage(&req, sizeof(req)) )
                {
                    CLIENT->sendMessage(encoder.getEncodedMessage(),
                                        encoder.getEncodedLen());
                    encoder.resetEncoder();
                    encoder.encodeMessage(&req, sizeof(req));
                }
            }
        }
    }

    if ( ! encoder.isEmpty() )
    {
        CLIENT->sendMessage(encoder.getEncodedMessage(),
                            encoder.getEncodedLen());
    }

    //sfx
    sound->playUnitSound(working_list.getHeadUnitType() );
    sound->playSound("move");
}

void
WorldInputCmdProcessor::sendAttackCommand(const iXY &world_pos)
{
    AttackUnitRequest req;

    UnitBase *target_ptr;

    size_t id_list_index;
    size_t id_list_size;
    UnitBase *unit_ptr;

    if ( working_list.isSelected() == true ) {
        target_list.selectTarget( world_pos );

        target_ptr = UnitInterface::getUnit( target_list.unit_list[0] );

        id_list_size = working_list.unit_list.size();

        if ( id_list_size == 0 )
            return;

        NetMessageEncoder encoder;

        for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ )
        {
            unit_ptr = UnitInterface::getUnit( working_list.unit_list[ id_list_index ] );
            if ( unit_ptr != 0 )
            {
                if ( unit_ptr->unit_state.select == true )
                {
                    req.setUnitId(unit_ptr->id);
                    req.setEnemyId(target_ptr->id);

                    if ( !encoder.encodeMessage(&req, sizeof(req)) )
                    {
                        CLIENT->sendMessage(encoder.getEncodedMessage(),
                                            encoder.getEncodedLen());
                        encoder.resetEncoder();
                        encoder.encodeMessage(&req, sizeof(req));
                    }
                }
            }
        }

        if ( ! encoder.isEmpty() )
        {
            CLIENT->sendMessage(encoder.getEncodedMessage(),
                                encoder.getEncodedLen());
        }

        //sfx
        sound->playSound("target");
    }
}

void
WorldInputCmdProcessor::sendManualFireCommand(const iXY &world_pos)
{
    if ( !actionTimer.isTimeOut() )
    {
       return;
    }

    actionTimer.reset();

    ManualFireRequest req;

    size_t id_list_index;
    size_t id_list_size;
    UnitBase *unit_ptr;

    if ( working_list.unit_list.size() > 0 ) {
        id_list_size = working_list.unit_list.size();

        NetMessageEncoder encoder;

        for( id_list_index = 0; id_list_index < id_list_size; id_list_index++ ) {
            unit_ptr = UnitInterface::getUnit( working_list.unit_list[ id_list_index ] );

            if ( unit_ptr != 0 ) {
                if ( unit_ptr->unit_state.select == true ) {
                    req.setUnitId(unit_ptr->id);
                    req.setMapPos(world_pos);

                    if ( !encoder.encodeMessage(&req, sizeof(req)) )
                    {
                        CLIENT->sendMessage(encoder.getEncodedMessage(),
                                            encoder.getEncodedLen());
                        encoder.resetEncoder();
                        encoder.encodeMessage(&req, sizeof(req));
                    }
                }
            }
        }

        if ( ! encoder.isEmpty() )
        {
            CLIENT->sendMessage(encoder.getEncodedMessage(),
                                encoder.getEncodedLen());
        }

        // SFX
        sound->playSound("target");
    } // ** if containsItems() > 0
}

void
WorldInputCmdProcessor::sendAllianceRequest(const iXY& world_pos, bool make_break)
{
    UnitBase *target_ptr;

    target_list.selectTarget(world_pos);

    if ( target_list.isSelected() == true )
    {
        target_ptr = UnitInterface::getUnit( target_list.unit_list[0] );

        if ( make_break )
        {
            AllianceRequest req;
            req.with_player_id = target_ptr->player_id;
            CLIENT->sendMessage( &req, sizeof(req));
        }
        else
        {
            BreakAllianceRequest req;
            req.with_player_id = target_ptr->player_id;
            CLIENT->sendMessage( &req, sizeof(req));
        }
    }
}

void
WorldInputCmdProcessor::process(bool process_mouse)
{
    evaluateKeyboardEvents();
    if ( process_mouse )
        evaluateMouseEvents();

    working_list.validateList();
}

void
WorldInputCmdProcessor::inFocus()
{
    iXY world_pos;
    iXY mouse_pos;

    MouseInterface::getMousePosition( &mouse_pos.x, &mouse_pos.y );

    WorldViewInterface::getViewWindow( &world_win );
    WorldViewInterface::clientXYtoWorldXY( world_win, mouse_pos, &world_pos );

    selection_box_active = false;
}

void
WorldInputCmdProcessor::draw()
{
    if (selection_box_active == true && box_press != box_release)
    {
        WorldViewInterface::getViewWindow(&world_win);
        iXY box1, box2;
        WorldViewInterface::worldXYtoClientXY(world_win, box_press, &box1);
        WorldViewInterface::worldXYtoClientXY(world_win, box_release, &box2);
        --box2.x;
        box2.y-=3;

        screen->drawRect(iRect(box1, box2), Color::white);
    }

    if (outpost_goal_selection != OBJECTIVE_NONE)
    {
        iXY mouse_pos;
        MouseInterface::getMousePosition( &mouse_pos.x, &mouse_pos.y );

        WorldViewInterface::getViewWindow( &world_win );
        iXY pos;
        WorldViewInterface::worldXYtoClientXY(world_win, output_pos_press,&pos);
        screen->drawLine(pos, mouse_pos, Color::blue);
    }

//    WorldViewInterface::getViewWindow(&world_win);

//    iXY mpos = MouseInterface::getMousePosition();
//    iXY wpos;
//    iXY mappos;

//    WorldViewInterface::clientXYtoWorldXY(world_win, mpos, &wpos);
//    MapInterface::pointXYtoMapXY( wpos, mappos );

//    iXY mappos2(mappos.x+1, mappos.y+1);

//    iXY wp1, wp2;
//    iXY r1, r2;

//    MapInterface::mapXYtoPointXY(mappos,  wp1);
//    MapInterface::mapXYtoPointXY(mappos2, wp2);

//    wp1.x -= 16;
//    wp1.y -= 16;
//    wp2.x -= 16;
//    wp2.y -= 16;

//    WorldViewInterface::worldXYtoClientXY(world_win, wp1, &r1);
//    WorldViewInterface::worldXYtoClientXY(world_win, wp2, &r2);

//    screen->drawRect(iRect(r1, r2), Color::white);
}

void
WorldInputCmdProcessor::closeSelectionBox()
{
    iXY world_pos;
    iXY mouse_pos;

    MouseInterface::getMousePosition( &mouse_pos.x, &mouse_pos.y );

    WorldViewInterface::getViewWindow( &world_win );
    WorldViewInterface::clientXYtoWorldXY( world_win, mouse_pos, &world_pos );

    if (selection_box_active == true) {
        selection_box_active = false;
        box_release = world_pos;
        left_button_hold_action_complete = selectBoundBoxUnits();
    }
}

bool
WorldInputCmdProcessor::isObjectiveSelected()
{
    if (Desktop::getVisible("VehicleSelectionView") == true) {
        return true;
    }

    return false;
}

const char*
WorldInputCmdProcessor::getSelectedObjectiveName()
{
    Objective *objective_state;

    objective_state = ObjectiveInterface::getObjective(selected_objective_id);

    return objective_state->name;
}

iXY
WorldInputCmdProcessor::getSelectedObjectiveWorldPos()
{
    Objective *objective_state;

    objective_state = ObjectiveInterface::getObjective(selected_objective_id);

    return objective_state->location;
}

void
WorldInputCmdProcessor::centerSelectedUnits()
{
    /** When you want to center the camera on a group of units you have the
     * problem of where to center the camera. The following nice idea+patch is
     * from Christian Hausknecht <christian.hausknecht@gmx.de>.
     *
     * I check, in which direction most of the units in that group go, and
     * based upon that pick up the unit, which is the most advanced one in this
     * direction. So in most cases you will have a focus on the right end of a
     * group (if that group is in a very long queue passing the map).
     */

    UnitBase *maxyunit = 0;
    UnitBase *maxxunit = 0;
    UnitBase *minyunit = 0;
    UnitBase *minxunit = 0;

    // Direction initialize
    int direction[8];
    for(int i=0;i<8;++i)
        direction[i]=0;

    // Vote direction
    bool firstunit = true;
    for(unsigned int id_list_index = 0; id_list_index < working_list.unit_list.size(); id_list_index++) {
        UnitBase* unit_ptr = UnitInterface::getUnit(working_list.unit_list[id_list_index]);

        if(unit_ptr == 0)
            continue;

            // increment one direction
        direction[unit_ptr->unit_state.orientation]+=1;
            // initialize some pointers, get them from first unit
        if(firstunit) {
            maxyunit = unit_ptr;
            maxxunit = unit_ptr;
            minyunit = unit_ptr;
            minxunit = unit_ptr;
            firstunit = false;
        } // choose extremest in each direction
        else {
            if(maxyunit->unit_state.location.y < unit_ptr->unit_state.location.y)
                maxyunit=unit_ptr;
            if(maxxunit->unit_state.location.x < unit_ptr->unit_state.location.x)
                maxxunit=unit_ptr;
            if(minyunit->unit_state.location.y > unit_ptr->unit_state.location.y)
                minyunit=unit_ptr;
            if(minxunit->unit_state.location.x > unit_ptr->unit_state.location.x)
                minxunit=unit_ptr;
        }
    }

    // Index of chosen direction (which is most used by all units of a group)
    int preferred_direction = 0;
    int max = -1;
    for(int i=0;i<8;++i) {
        if(direction[i] > max) {
            max=direction[i];
            preferred_direction = i;
        }
    }

    // Chose Best unit correspondig to chosen direction
    UnitBase* unit = 0;
    switch(preferred_direction) {
      case 0:
          unit = maxxunit;
          break;
      case 1:
          if(direction[0]>direction[2])
              unit = maxxunit;
          else
              unit = minyunit;
          break;
      case 2:
          unit = minyunit;
          break;
      case 3:
          if(direction[4]>direction[2])
              unit = minxunit;
          else
              unit = minyunit;
          break;
      case 4:
          unit = minxunit;
          break;
      case 5:
          if(direction[4]>direction[6])
              unit = minxunit;
          else
              unit = maxyunit;
          break;
      case 6:
          unit = maxyunit;
          break;
      case 7:
          if(direction[6]>direction[0])
              unit = maxyunit;
          else
              unit = maxxunit;
          break;
      default:
          assert(false);
          break;
    }

    if(unit != 0)
        WorldViewInterface::setCameraPosition(unit->unit_state.location);
}
