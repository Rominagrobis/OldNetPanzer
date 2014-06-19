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
 * Created on September 21, 2012, 5:06 PM
 */

#ifndef STARTSURRENDERVOTEACTION_HPP
#define	STARTSURRENDERVOTEACTION_HPP

#include "Action.hpp"
#include "Network/PlayerRequests/StartSurrenderVoteRequest.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Views/Components/Desktop.hpp"

class StartSurrenderVoteAction : public Action
{
public:
    StartSurrenderVoteAction() : Action(true) {}
    
    void execute()
    {
        if ( !Desktop::getVisible("PrepareTeam") && !Desktop::getVisible("GFlagSelectionView") )
        {
            StartSurrenderVoteRequest req;
            CLIENT->sendMessage(&req, sizeof(req));
        }
    }
};

#endif	/* STARTSURRENDERVOTEACTION_HPP */

