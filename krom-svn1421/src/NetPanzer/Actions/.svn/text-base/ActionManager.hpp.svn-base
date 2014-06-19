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
 * Created on September 3, 2012, 7:13 PM
 */

#ifndef ACTIONMANAGER_HPP
#define	ACTIONMANAGER_HPP

#include "Core/CoreTypes.hpp"

class Action;

class ActionManager
{
public:
    static void initialize();
    static void deinitialize();
    
    static void addSystemActions();
    static void addGUIActions();

    static bool addAction(const NPString& name, Action* action);
    static void deleteAction(const NPString& name);
    
    static Action* getAction(const NPString& name);
    
    static bool runAction(const NPString& name);
    
};

#endif	/* ACTIONMANAGER_HPP */

