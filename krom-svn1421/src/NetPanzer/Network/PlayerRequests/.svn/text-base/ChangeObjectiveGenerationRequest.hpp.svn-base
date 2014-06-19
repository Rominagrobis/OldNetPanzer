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
 * Created on September 19, 2012, 10:18 AM
 */

#ifndef CHANGEOBJECTIVEGENERATIONREQUEST_HPP
#define	CHANGEOBJECTIVEGENERATIONREQUEST_HPP

#include "PlayerRequests.hpp"
#include "Classes/Network/NetMessage.hpp"

class ChangeObjectiveGenerationRequest : public NetMessage
{
public:
    ObjectiveID objective_id;
    Uint8 unit_type;
    Uint8 unit_gen_on;

    ChangeObjectiveGenerationRequest()
    {
        message_class = _net_message_class_player_commands;
        message_id = PlayerRequests::CHANGE_OBJECTIVE_GENERATION;
    }

    void setObjectiveId(ObjectiveID id) { objective_id = ObjectiveID_toPortable(id); }
    ObjectiveID getObjectiveId() const  { return ObjectiveID_fromPortable(objective_id); }
    
} __attribute__((packed));

#endif	/* CHANGEOBJECTIVEGENERATIONREQUEST_HPP */

