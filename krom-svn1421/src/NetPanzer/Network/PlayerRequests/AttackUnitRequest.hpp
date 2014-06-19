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
 * Created on September 19, 2012, 8:41 AM
 */

#ifndef ATTACKUNITREQUEST_HPP
#define	ATTACKUNITREQUEST_HPP

#include "PlayerRequests.hpp"
#include "Classes/Network/NetMessage.hpp"

class AttackUnitRequest : public NetMessage
{
private:
    Uint16 unit_id;
    Uint16 enemy_id;
    
public:
    AttackUnitRequest()
    {
        message_class = _net_message_class_player_commands;
        message_id = PlayerRequests::ATTACK_UNIT;
    }
    
    void setUnitId(const UnitID unit_id) { this->unit_id = UnitID_toPortable(unit_id); }
    UnitID getUnitId() const { return UnitID_fromPortable(unit_id); }
    
    void setEnemyId(const UnitID enemy_id) { this->enemy_id = UnitID_toPortable(enemy_id); }
    UnitID getEnemyId() const { return UnitID_fromPortable(enemy_id); }
    
} __attribute__((packed));


#endif	/* ATTACKUNITREQUEST_HPP */

