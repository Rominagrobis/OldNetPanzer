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
 * Created on September 2, 2012, 1:48 AM
 */

#ifndef ACTION_HPP
#define	ACTION_HPP

class Action
{
private:
    Action(const Action&);
    void operator=(const Action&);
    
protected:
    bool shared;
    
public:
    Action(bool shared) : shared(shared) {}
    virtual ~Action() {}
    
    bool isShared() const { return shared; }
    
    virtual void execute() = 0;
};

#endif	/* ACTION_HPP */

