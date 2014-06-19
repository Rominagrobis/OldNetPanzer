/*
Copyright (C) 2003 Ivo Danihelka <ivo@danihelka.net>

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

#include "Bot.hpp"

#include "Classes/PlacementMatrix.hpp"
#include "Units/UnitBase.hpp"
#include "Objectives/Objective.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Util/Log.hpp"

#include "Network/PlayerRequests/MoveUnitRequest.hpp"
#include "Network/PlayerRequests/AttackUnitRequest.hpp"
#include "Network/PlayerRequests/ManualFireRequest.hpp"
#include "Network/PlayerRequests/ChangeObjectiveGenerationRequest.hpp"


Bot *Bot::s_bot = 0;
//-----------------------------------------------------------------
void
Bot::initialize(Bot *bot)
{
    s_bot = bot;
}
//-----------------------------------------------------------------
void
Bot::shutdown()
{
    if (s_bot) {
        delete s_bot;
        s_bot = 0;
    }
}
//-----------------------------------------------------------------
void
Bot::moveUnit(UnitBase *unit, iXY map_pos)
{
    assert(unit != 0);

    PlacementMatrix matrix;
    matrix.reset(map_pos);
    matrix.getNextEmptyLoc(&map_pos);

    MoveUnitRequest comm_mesg;
    comm_mesg.setUnitId(unit->id);
    comm_mesg.setMapPos(map_pos);

    CLIENT->sendMessage(&comm_mesg, sizeof(comm_mesg));
    m_tasks.setUnitTask(unit, BotTaskList::TASK_MOVE);

    LOGGER.debug("bot: moveUnit %d to %dx%d", unit->id, map_pos.x, map_pos.y);
}
//-----------------------------------------------------------------
void
Bot::attackUnit(UnitBase *unit, UnitBase *enemyUnit)
{
    assert(unit != 0);
    assert(enemyUnit != 0);

    AttackUnitRequest comm_mesg;
    comm_mesg.setUnitId(unit->id);
    comm_mesg.setEnemyId(enemyUnit->id);

    CLIENT->sendMessage(&comm_mesg, sizeof(comm_mesg));
    m_tasks.setUnitTask(unit, BotTaskList::TASK_ATTACK);

    LOGGER.debug("bot: attackUnit %d to %d", unit->id, enemyUnit->id);
}
//-----------------------------------------------------------------
void
Bot::manualFire(UnitBase *unit, iXY world_pos)
{
    assert(unit != 0);

    ManualFireRequest comm_mesg;
    comm_mesg.setUnitId(unit->id);
    comm_mesg.setMapPos(world_pos);

    CLIENT->sendMessage(&comm_mesg, sizeof(comm_mesg));
    //NOTE: manual fire is not special unit task,
    // unit can move and fire simultanous
}
//-----------------------------------------------------------------
void
Bot::produceUnit(ObjectiveID outpostID, int selectedProduce)
{
    LOGGER.debug("bot: produceUnit outpost=%d selectedProduce=%d",
                 outpostID, selectedProduce);

    ChangeObjectiveGenerationRequest req;
    req.setObjectiveId(outpostID);
    req.unit_type = selectedProduce;
    req.unit_gen_on = 1;
    
    CLIENT->sendMessage(&req, sizeof(req));
}

