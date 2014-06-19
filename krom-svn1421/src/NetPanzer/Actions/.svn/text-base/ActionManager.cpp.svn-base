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
 * Created on September 4, 2012, 7:36 PM
 */

#include "ActionManager.hpp"

#include "Action.hpp"
#include "Core/CoreTypes.hpp"
#include "Util/Log.hpp"

#include <map>

static std::map<NPString, Action*> * actions = 0;

void ActionManager::initialize()
{
    actions = new std::map<NPString, Action*>();
    
    addSystemActions();
}

void ActionManager::deinitialize()
{
    if ( actions )
    {
        Action * a;
        std::map<NPString, Action*>::iterator i;
        
        for ( i = actions->begin(); i != actions->end(); i = actions->begin() )
        {
            a = i->second;
            for ( ; i != actions->end() ; )
            {
                if ( i->second == a )
                {
                    actions->erase(i++);
                }
                else
                {
                    ++i;
                }
            }
            delete a;
        }

        delete actions;
        actions = 0;
    }
}

bool ActionManager::addAction(const NPString& name, Action* action)
{
    std::map<NPString, Action*>::iterator i = actions->find(name);
    if ( i != actions->end() )
    {
        return false;
    }
    
    actions->insert(std::make_pair(name, action));
    return true;
}

void ActionManager::deleteAction(const NPString& name)
{
    std::map<NPString, Action*>::iterator i = actions->find(name);
    if ( i != actions->end() )
    {
        delete i->second;
        actions->erase(i);
    }
}

Action* ActionManager::getAction(const NPString& name)
{
    std::map<NPString, Action*>::iterator i = actions->find(name);
    if ( i != actions->end() )
    {
        return i->second;
    }
    
    LOGGER.warning("Action undefined: '%s'", name.c_str());
    
    return 0;
}

bool ActionManager::runAction(const NPString& name)
{
    std::map<NPString, Action*>::iterator i = actions->find(name);
    if ( i != actions->end() )
    {
        i->second->execute();
        return true;
    }
    
    return false;
}
