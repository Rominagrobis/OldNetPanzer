/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Aaron Perez

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
 *
 * Created on September 19, 2012, 8:38 AM
 */

#ifndef MOVEUNITREQUEST_HPP
#define	MOVEUNITREQUEST_HPP

#include "PlayerRequests.hpp"
#include "Classes/Network/NetMessage.hpp"

class MoveUnitRequest : public NetMessage
{
private:
    Uint16 unit_id;
    Uint16 map_x;
    Uint16 map_y;
    
public:
    MoveUnitRequest()
    {
        message_class = _net_message_class_player_commands;
        message_id = PlayerRequests::MOVE_UNIT;
    }
    
    void setUnitId(const UnitID unit_id) { this->unit_id = UnitID_toPortable(unit_id); }
    UnitID getUnitId() const { return UnitID_fromPortable(unit_id); }

    void setMapPos(const iXY& map_pos)
    {
        map_x = htol16(map_pos.x);
        map_y = htol16(map_pos.y);
    }
    
    iXY getMapPos() const
    {
        return iXY(ltoh16(map_x), ltoh16(map_y));
    }
    
} __attribute__((packed));

#endif	/* MOVEUNITREQUEST_HPP */

