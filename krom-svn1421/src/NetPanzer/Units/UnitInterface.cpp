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


#include <iostream>
#include <algorithm>

#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "Interfaces/GameConfig.hpp"

#include "Units/Vehicle.hpp"

#include "Types/iXY.hpp"
#include "Util/Timer.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetMessageEncoder.hpp"
#include "Interfaces/Console.hpp"
#include "Units/UnitOpcodeDecoder.hpp"

#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetPacket.hpp"

#include "System/Sound.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Util/Log.hpp"
#include "UnitBlackBoard.hpp"

//UnitList * UnitInterface::unit_lists;
UnitInterface::Units UnitInterface::units;
UnitInterface::PlayerUnitList* UnitInterface::playerUnitLists = 0;
UnitBucketArray UnitInterface::unit_bucket_array;

PlayerID UnitInterface::max_players;
PlacementMatrix UnitInterface::unit_placement_matrix;

UnitID UnitInterface::lastUnitID;

UnitOpcodeEncoder UnitInterface::opcode_encoder;

Timer UnitInterface::message_timer;
Timer UnitInterface::no_guarantee_message_timer;

unsigned long   UnitInterface::sync_units_iterator;
bool	        UnitInterface::sync_units_complete_flag;
unsigned short  UnitInterface::sync_units_list_index;
unsigned long   UnitInterface::sync_units_total_units;
unsigned long   UnitInterface::sync_units_in_sync_count;
unsigned long   UnitInterface::sync_units_in_sync_partial_count;
size_t          UnitInterface::units_per_player;
Timer		UnitInterface::sync_units_packet_timer;


// ******************************************************************

void UnitInterface::initialize( unsigned long max_units )
{
    max_players = PlayerInterface::getMaxPlayers();

    //unit_lists = new UnitList [ max_players ];
    playerUnitLists = new PlayerUnitList[max_players];

    unit_bucket_array.initialize(MapInterface::getSize(), TileInterface::getTileSize() );

    lastUnitID = 0;
    message_timer.changeRate( 8 );
    no_guarantee_message_timer.changeRate( 15 );

    units_per_player = max_units;
}

// ******************************************************************

void UnitInterface::cleanUp()
{
    // XXX why doesn't clean the unit_bucket_array?
    delete[] playerUnitLists;
    playerUnitLists = 0;

    for(Units::iterator i = units.begin(); i != units.end(); ++i)
        delete i->second;
    units.clear();
}

void
UnitInterface::reset()
{
    for(size_t i = 0; i < max_players; i++ ) {
        playerUnitLists[i].clear();
    }

    unit_bucket_array.initialize( MapInterface::getSize(), TileInterface::getTileSize() );

    for(Units::iterator i = units.begin(); i != units.end(); ++i)
        delete i->second;
    units.clear();
}

void
UnitInterface::playerCommand_MoveUnit(const PlayerID player_id, const UnitID unit_id, const iXY& destination)
{
    UnitBase * unit = getUnit(unit_id);
    
    if ( unit )
    {
        if ( unit->player_id == player_id )
        {
            unit->moveToLoc(destination);
        }
    }
}

void
UnitInterface::playerCommand_AttackUnit(const PlayerID player_id, const UnitID unit_id, const UnitID enemy_id)
{
    UnitBase * unit = getUnit(unit_id);
    
    if ( unit )
    {
        if ( unit->player_id == player_id )
        {
            unit->attackUnit(unit_id);
        }
    }
}

void
UnitInterface::playerCommand_ManualShoot(const PlayerID player_id, const UnitID unit_id, const iXY& world_point)
{
    UnitBase * unit = getUnit(unit_id);
    
    if ( unit )
    {
        if ( unit->player_id == player_id )
        {
            unit->manualShoot(world_point);
        }
    }
}

// ******************************************************************

void UnitInterface::removeUnit(Units::iterator i)
{
    UnitBase* unit = i->second;
    
    // unit explosion particles
    ParticleInterface::addHit(unit->unit_state);
    
    // unit explosion sound
    sound->playAmbientSound("expl",                   
            WorldViewInterface::getCameraDistance(
                unit->unit_state.location ) );
    
    // delete the unit
    unit_bucket_array.deleteUnitBucketPointer(unit->id, 
            unit->unit_state.location );
    PlayerUnitList& plist =
        playerUnitLists[unit->player_id];
    
    PlayerUnitList::iterator pi
        = std::find(plist.begin(), plist.end(), unit);
    assert(pi != plist.end());
    if(pi != plist.end())
        plist.erase(pi);

    units.erase(i);
    delete unit;
}

// ******************************************************************

void UnitInterface::updateUnitStatus()
{
    for(Units::iterator i = units.begin(); i != units.end(); /*nothing*/ ) {
        UnitBase* unit = i->second;
	    
        if (unit->unit_state.lifecycle_state == _UNIT_LIFECYCLE_INACTIVE) {
            Units::iterator next = i;
            ++next;
            removeUnit(i);
            i = next;
            continue;
        }
	    
        unsigned long pre_update_bucket_index;
        unsigned long post_update_bucket_index;

        pre_update_bucket_index 
            = unit_bucket_array.worldLocToBucketIndex(
                    unit->unit_state.location );
        unit->updateState();

        post_update_bucket_index 
            = unit_bucket_array.worldLocToBucketIndex(
                    unit->unit_state.location );

        if ( post_update_bucket_index != pre_update_bucket_index ) {
            unit_bucket_array.moveUnit(unit->id,
                    pre_update_bucket_index, post_update_bucket_index );
        }
        ++i;
    }

    if ( NetworkState::status == _network_state_server ) {
        if (message_timer.count()) {
            opcode_encoder.send();
        }
    }
}

void UnitInterface::offloadGraphics(SpriteSorter& sorter)
{
    iRect world_window_rect;
    iRect bucket_rect;
    UnitBucketList *bucket_list;

    world_window_rect = sorter.getWorldWindow();
    bucket_rect = unit_bucket_array.worldRectToBucketRectClip(world_window_rect);

    for(long row_index = bucket_rect.min.y;
            row_index <= bucket_rect.max.y; row_index++ ) {
        for(long column_index = bucket_rect.min.x;
                column_index <= bucket_rect.max.x; column_index++ ) {
            bucket_list = unit_bucket_array.getBucket(row_index, column_index);

            for(UnitBucketPointer* t = bucket_list->getFront();
                    t != 0; t = t->next) {
                t->unit->offloadGraphics(sorter);
            }
        }
    }
}

// ******************************************************************

UnitID UnitInterface::newUnitID()
{
    UnitID newID = lastUnitID++;
    while(getUnit(newID) != 0)
        newID = lastUnitID++;

    return newID;
}

// ******************************************************************

UnitBase * UnitInterface::newUnit( unsigned short unit_type,
                                   const iXY &location,
                                   PlayerID player_index,
                                   UnitID id)
{
    UnitBase* unit = 0;

    PlayerState* player = PlayerInterface::getPlayer( player_index );

    if ( unit_type < UnitProfileInterface::getNumUnitTypes() )
    {
        unit = new Vehicle(player, unit_type, id, location);        
    }
    else
    {   // XXX change for a error window
        assert("unknown unit_type" == 0);        
    }

    return unit;
}

// ******************************************************************

void UnitInterface::addNewUnit(UnitBase *unit)
{
    units.insert(std::make_pair(unit->id, unit));
   
    Uint16 player_index = unit->player_id;
    playerUnitLists[player_index].push_back(unit);

    unit_bucket_array.addUnit(unit);
}

// ******************************************************************

void UnitInterface::sortBucketArray()
{
    unsigned long bucket_count;
    unsigned long bucket_index;
    UnitBucketList *bucket_list;
    UnitBucketPointer *traversal_ptr;

    bucket_count = unit_bucket_array.getSize();

    for( bucket_index = 0; bucket_index < bucket_count; bucket_index++ ) {
        bucket_list = unit_bucket_array.getBucket( bucket_index );

        traversal_ptr = bucket_list->getFront();

        while( traversal_ptr != 0 ) {
            unsigned long unit_bucket_index;

            unit_bucket_index = unit_bucket_array.worldLocToBucketIndex( traversal_ptr->unit->unit_state.location );

            if( unit_bucket_index != bucket_index ) {
                unit_bucket_array.moveUnit( traversal_ptr->unit->id,
                        bucket_index, unit_bucket_index );
            }

            traversal_ptr = traversal_ptr->next;
        } // ** while
    }
}

// ******************************************************************

UnitBase*
UnitInterface::getUnit(UnitID id)
{
    Units::iterator i = units.find(id);
    if(i == units.end()) {
        return 0;
    }

    return i->second;
}

// ******************************************************************
UnitBase* UnitInterface::createUnit( unsigned short unit_type,
                                      const iXY &location,
                                      PlayerID player_id)
{
    if (playerUnitLists[player_id].size() >= units_per_player)
	return 0;

    UnitBase* unit = newUnit(unit_type, location, player_id, newUnitID());
    addNewUnit(unit);

    return unit;
}

// ******************************************************************

void UnitInterface::spawnPlayerUnits(const iXY &location,
                                     PlayerID player_id,
                                     const PlayerUnitConfig &unit_config)
{
    iXY next_loc;
    UnitBase *unit;
    unsigned long unit_type_index;
    unsigned long unit_spawn_count;
    unsigned long unit_spawn_index;

    NetMessageEncoder encoder;

    unit_placement_matrix.reset( location );

    for ( unit_type_index = 0; unit_type_index < UnitProfileInterface::getNumUnitTypes(); unit_type_index++ )
    {

        unit_spawn_count = unit_config.getSpawnUnitCount( unit_type_index );
        for ( unit_spawn_index = 0; unit_spawn_index < unit_spawn_count; unit_spawn_index++ )
        {
            unit_placement_matrix.getNextEmptyLoc( &next_loc );
            unit = createUnit(unit_type_index, next_loc, player_id);
            if (unit == 0) return;
            UnitRemoteCreate create_mesg(unit->player_id, unit->id,
                    next_loc.x, next_loc.y, unit->unit_state.unit_type);
            if ( !encoder.encodeMessage(&create_mesg, sizeof(create_mesg)) )
            {
                LOGGER.info("UnitInterface encoder full, sending and resetting");
                SERVER->broadcastMessage(encoder.getEncodedMessage(),
                                         encoder.getEncodedLen());
                encoder.resetEncoder();
                encoder.encodeMessage(&create_mesg, sizeof(create_mesg));
            }
        } // ** for unit_spawn_index
    } // ** for unit_type_index

    if ( ! encoder.isEmpty() )
    {
        SERVER->broadcastMessage(encoder.getEncodedMessage(),
                                 encoder.getEncodedLen());
    }
}

// ******************************************************************

void
UnitInterface::queryUnitsAt(std::vector<UnitID>& working_list,
        const iXY& point, PlayerID player_id, unsigned char search_flags)
{
    for(Units::iterator i = units.begin(); i != units.end(); ++i) {
        UnitBase* unit = i->second;
        if(!unit->unit_state.bounds(point))
            continue;

        if(search_flags == _search_exclude_player
                && unit->player_id == player_id)
            continue;
        if(search_flags == _search_player
                && unit->player_id != player_id)
            continue;

        working_list.push_back(unit->id);
    }
}

// ******************************************************************

void
UnitInterface::queryUnitsAt(std::vector<UnitID>& working_list,
        const iRect& rect, PlayerID player_id, unsigned char search_flags)
{
    for(Units::iterator i = units.begin(); i != units.end(); ++i) {
        UnitBase* unit = i->second;
        if(!rect.contains(unit->unit_state.location))
            continue;

        if(search_flags == _search_exclude_player
                && unit->player_id == player_id)
            continue;
        if(search_flags == _search_player
                && unit->player_id != player_id)
            continue;

        working_list.push_back(unit->id);
    }
}

PlayerState *
UnitInterface::querySinglePlayerInArea(const iRect& rect)
{
    PlayerID player_id = INVALID_PLAYER_ID;
    for ( Units::iterator i = units.begin(); i != units.end(); ++i )
    {
        UnitBase* unit = i->second;
        if ( !rect.contains(unit->unit_state.location) )
        {
            continue;
        }

        if ( player_id != INVALID_PLAYER_ID && unit->player_id != player_id )
        {
            return 0;
        }
        player_id = unit->player_id;
    }
    return (player_id == INVALID_PLAYER_ID) ? 0 : PlayerInterface::getPlayer(player_id);
}

/****************************************************************************/

bool UnitInterface::queryClosestUnit( UnitBase **closest_unit_ptr,
                                       iXY &loc, PlayerID player_id,
                                       unsigned char search_flags )
{
    long closest_magnitude = 0;
    UnitBase* closest_unit = 0;

    for(Units::iterator i = units.begin(); i != units.end(); ++i) {
        UnitBase* unit = i->second;

        if(search_flags == _search_exclude_player
                && unit->player_id == player_id)
            continue;
        if(search_flags == _search_player
                && unit->player_id != player_id)
            continue;

        iXY delta;
        long temp_mag;

        if ( closest_unit == 0 ) {
            closest_unit = unit;
            delta  = loc - unit->unit_state.location;
            closest_magnitude = long(delta.mag2());
        } else {
            delta  = loc - unit->unit_state.location;
            temp_mag = long(delta.mag2());
            
            if ( closest_magnitude > temp_mag ) {
                closest_unit = unit;
                closest_magnitude = temp_mag;
            }
        }
    }

    if(closest_unit != 0) {
        *closest_unit_ptr = closest_unit;
        return true;
    }

    *closest_unit_ptr = 0;
    return false;
}

bool UnitInterface::queryClosestUnit( UnitBase **closest_unit_ptr, iRect &bounding_rect, iXY &loc )
{
    UnitBase *closest_unit = 0;
    long closest_magnitude = 0;
    iRect bucket_rect;
    UnitBucketList *bucket_list;
    UnitBucketPointer *traversal_ptr;

    bucket_rect = unit_bucket_array.worldRectToBucketRect( bounding_rect );

    for( long row_index = bucket_rect.min.y; row_index <= bucket_rect.max.y; row_index++ ) {
        for( long column_index = bucket_rect.min.x; column_index <= bucket_rect.max.x; column_index++ ) {
            bucket_list = unit_bucket_array.getBucket( row_index, column_index );

            traversal_ptr = bucket_list->getFront();

            while( traversal_ptr != 0 ) {
                iXY delta;
                long temp_mag;

                if ( closest_unit == 0 ) {
                    closest_unit = traversal_ptr->unit;
                    delta  = loc - traversal_ptr->unit->unit_state.location;
                    closest_magnitude = long(delta.mag2());
                } else {
                    delta  = loc - traversal_ptr->unit->unit_state.location;
                    temp_mag = long(delta.mag2());

                    if ( closest_magnitude > temp_mag ) {
                        closest_unit = traversal_ptr->unit;
                        closest_magnitude = temp_mag;
                    }
                }

                traversal_ptr = traversal_ptr->next;
            }
        }
    }

    if( closest_unit != 0 ) {
        *closest_unit_ptr = closest_unit;
        return true;
    }

    *closest_unit_ptr = 0;
    return false;
}

// ******************************************************************

bool UnitInterface::queryClosestEnemyUnit(UnitBase **closest_unit_ptr,
        iXY &loc, PlayerID player_index)
{
    UnitBase *closest_unit = 0;
    long closest_magnitude = 0;

    for(Units::iterator i = units.begin(); i != units.end(); ++i) {
        UnitBase* unit = i->second;
        PlayerID unitPlayerID = unit->player_id;
        
        if(unitPlayerID == player_index
                || PlayerInterface::isAllied(player_index, unitPlayerID) // XXX ALLY
                )
            continue;

        iXY delta;
        long temp_mag;

        if ( closest_unit == 0 ) {
            closest_unit = unit;
            delta  = loc - unit->unit_state.location;
            closest_magnitude = long(delta.mag2());
        } else {
            delta  = loc - unit->unit_state.location;
            temp_mag = long(delta.mag2());
            
            if ( closest_magnitude > temp_mag ) {
                closest_unit = unit;
                closest_magnitude = temp_mag;
            }
        }
    }

    if( closest_unit != 0 ) {
        *closest_unit_ptr = closest_unit;
        return true;
    }

    *closest_unit_ptr = 0;
    return false;
}

// ******************************************************************

unsigned char UnitInterface::queryUnitLocationStatus(iXY loc)
{
    PlayerID player_id = PlayerInterface::getLocalPlayerIndex();

    std::vector<UnitID> locUnits;
    queryUnitsAt(locUnits, loc, player_id, 0);
    if(locUnits.size() == 0) {
        return _no_unit_found;
    }

    UnitID id = locUnits[0];
    UnitBase* unit = getUnit(id);
    if(!unit) {
        return _no_unit_found;
    }
    if(unit->player_id == player_id) {
        return _unit_player;
    }
    // XXX ALLY
    if(PlayerInterface::isAllied(player_id, unit->player_id)) {
        return _unit_allied;
    }

    return _unit_enemy;
}

// ******************************************************************

bool UnitInterface::queryUnitAtMapLoc(iXY map_loc, UnitID *queary_unit_id)
{
    iXY unit_map_loc;

    for(Units::iterator i = units.begin(); i != units.end(); ++i) {
        UnitBase* unit = i->second;
        UnitState* unit_state = & unit->unit_state;
            
        MapInterface::pointXYtoMapXY( unit_state->location, unit_map_loc );
        if( map_loc == unit_map_loc ) {
            *queary_unit_id = unit->id;
            return true;
        }
    }

    return false;
}

// ******************************************************************

void UnitInterface::unitKilled(const UnitBase* unit, const UnitID killer)
{
    UnitBase* unit2 = getUnit(killer);
    if ( ! unit2 )
    {
        LOGGER.warning(
                "Killer Unit in EndLifeCycle message doesn't exist anymore(u1: %u u2: %u)",
                unit->id, killer);
        return;
    }
    
    PlayerState* player1 = PlayerInterface::getPlayer(unit->player_id);
    PlayerState* player2 = PlayerInterface::getPlayer(unit2->player_id);

    int unittype1 = unit->unit_state.unit_type;
    const std::string& unitname1 = UnitProfileInterface::getUnitProfile(unittype1)->unitname;
    
    int unittype2 = unit2->unit_state.unit_type;
    const std::string& unitname2 = UnitProfileInterface::getUnitProfile(unittype2)->unitname;
    
    if(Console::server)
    {
        *Console::server << "'" << player1->getName() << "' killed a '"
            << unitname2 << "' from '" << player2->getName() 
            << "' with his '" << unitname1 << "'." << std::endl;
    }

    // killing team own units doesn't give score
    if (GameConfig::game_teammode && (player1->getTeamID() == player2->getTeamID()))
    {
        return;
    }
    
    // killing own units doesn't give score
    if( player1 != player2 ) 
    {
        PlayerInterface::setKill(player1, player2, (UnitType) unittype1);
        
        PlayerScoreUpdate score_update;
        score_update.set(player1->getID(), player2->getID(), (UnitType) unittype1);
        
        SERVER->broadcastMessage(&score_update, sizeof(PlayerScoreUpdate));
    }
}


// ******************************************************************

void UnitInterface::unitOpcodeMessage(const NetMessage *net_message, size_t size)
{
    UnitOpcodeDecoder decoder;
    decoder.setMessage(net_message, size);

    UnitOpcode* opcode;
    while(decoder.decode(&opcode)) {
        UnitBase* unit = getUnit(opcode->getUnitID());

        if(!unit) {
            LOGGER.debug("Update for non-existant unit: %d",
                    opcode->getUnitID());
            continue;
        }
        
        unit->evalCommandOpcode(opcode);
    }
}

// ******************************************************************

void UnitInterface::unitDestroyMessage(const NetMessage *net_message)
{
    const UnitRemoteDestroy* remote_destroy 
        = (const UnitRemoteDestroy *) net_message;

    Units::iterator i = units.find(remote_destroy->getUnitToDestroy());
    if(i != units.end()) {
        removeUnit(i);
    }
}

// ******************************************************************

void UnitInterface::unitCreateMessage(const NetMessage* net_message)
{
    const UnitRemoteCreate* create_mesg 
        = (const UnitRemoteCreate *) net_message;

    PlayerID player_index = create_mesg->getPlayerID();

    try {
        std::map<UnitID, UnitBase*>::iterator uit = units.find(create_mesg->getUnitID());
        if ( uit != units.end() ) {
            LOGGER.warning("UnitInterface::unitCreateMessage() Received an existing unit [%d]",
                            create_mesg->getUnitID());
            return;
        }
        iXY unitpos(create_mesg->getLocX(), create_mesg->getLocY());
        UnitBase* unit = newUnit(create_mesg->unit_type, unitpos,
                                 player_index, create_mesg->getUnitID());
        addNewUnit(unit);
        // remove unit from blackboard in client (we are client here)
        UnitBlackBoard::unmarkUnitLoc( unitpos );
    } catch(std::exception& e) {
        LOGGER.warning("UnitInterface::unitSyncMessage() Couldn't create new unit '%s'", e.what());
    }
}

// ******************************************************************

void UnitInterface::unitSyncIntegrityCheckMessage(const NetMessage* )
{
    sortBucketArray();
}

// ******************************************************************
void UnitInterface::processNetMessage(const NetMessage* net_message, size_t size)
{
    switch(net_message->message_id)  {

        case _net_message_id_opcode_mesg:
            unitOpcodeMessage(net_message, size);
            break;

        case _net_message_id_destroy_unit:
            unitDestroyMessage(net_message);
            break;

        case _net_message_id_create_unit:
            unitCreateMessage(net_message);
            break;

        case _net_message_id_unit_sync_integrity_check:
            unitSyncIntegrityCheckMessage(net_message);
            break;

        default:
            LOGGER.warning("Unknown message id in UnitMessage (%d)",
                    net_message->message_id);
#ifdef DEBUG
            assert(false);
#endif
            break;
    }
}

// ******************************************************************

void UnitInterface::selfDestructUnit(const UnitID unit_id)
{
    UnitBase * unit = getUnit(unit_id);
    if ( unit )
    {
        unit->selfDestruct();
    }
}

void UnitInterface::destroyPlayerUnits(PlayerID player_id)
{
    PlayerUnitList& unitlist = playerUnitLists[player_id];
    for(PlayerUnitList::iterator i = unitlist.begin();
            i != unitlist.end(); ++i)
    {
        UnitBase* unit = *i;
        unit->selfDestruct();
    }
}

void UnitInterface::weaponHit(  const UnitID from_unit,
                                const iXY& location,
                                const Uint16 damage_factor)
{
    for(Units::iterator i = units.begin(); i != units.end(); ++i)
    {
        UnitBase* unit = i->second;
        if ( unit->unit_state.bounds(location) )
        {
            if ( unit->weaponHit( from_unit, damage_factor) )
            {
                unitKilled(unit, from_unit);
            }
        }
    }
}

