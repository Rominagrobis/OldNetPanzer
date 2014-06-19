/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Laurent Jacques

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

#include "Views/Components/tVScrollBar.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"

#define bSize (15)

void tVScrollBar::initScrollBar()
{
    Surface bitmap;
    bitmap.loadBMP(itScroll);
    bRiseOver.grab(bitmap, iRect(bSize*4, bSize, bSize*5, bSize*2));
    bRiseNormal.grab(bitmap, iRect(bSize*4, 0, bSize*5, bSize));
    bUpOver.grab(bitmap, iRect(bSize*2, 0, bSize*3, bSize));
    bUpNormal.grab(bitmap, iRect(bSize*3, 0, bSize*4, bSize));
    bDownOver.grab(bitmap, iRect(0, 0, bSize, bSize));
    bDownNormal.grab(bitmap, iRect(bSize, 0, bSize*2, bSize));
    setLocation(iXY(0,0));
    setSize(bSize, 0);

    Position = 0;
    Min = 0;
    Max = 100;
    RisePos = 0;
    bRisestate = NORMAL;
    bUpstate = NORMAL;
    bDownstate = NORMAL;
    background = ctWindowsbackground;
    SmallChange = 1;
    LargeChange = 10;
}
tVScrollBar::tVScrollBar()
{
    initScrollBar();
}

tVScrollBar::tVScrollBar(iXY pos, int Height, StateChangedCallback* newcallback)
{
    callback = newcallback;
    setLocation(pos);
    setSize(bSize, Height);
    initScrollBar();
    dirty = true;
}

void tVScrollBar::actionPerformed(const mMouseEvent &me)
{
    if (!enabled) return;
    iRect rect_button_up(position.x, position.y, position.x+bSize, position.y+bSize);
    iRect rect_button_Down(position.x, position.y+size.y-bSize, position.x+bSize, position.y+size.y);
    
    if (rect_button_up.contains(iXY(me.getX(), me.getY())))
    {
        bUpstate = OVER;
        dirty = true;
        if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
        {
            bUpstate = PRESSED;
            if (Position-SmallChange > Min) setPosition(Position-SmallChange);
            else setPosition(Min);
            if(callback)
                callback->stateChanged(this);
            return;
        }
    }
    else
    {
        bUpstate = NORMAL;
        dirty = true;
    }

    if (rect_button_Down.contains(iXY(me.getX(), me.getY())))
    {
        bDownstate = OVER;
        dirty = true;
        if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
        {
            bDownstate = PRESSED;
            if (Position+SmallChange < Max) setPosition(Position+SmallChange);
            else setPosition(Max);
            if(callback)
                callback->stateChanged(this);
            return;
        }
    }
    else
    {
        bDownstate = NORMAL;
        dirty = true;
    }

    int bar_min = position.y+bSize+RisePos;
    int bar_max = bar_min + bSize;
    
    if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
    {
        if ( me.getY() < bar_min ) 
        {
            if (Position-LargeChange > Min) setPosition(Position-LargeChange);
            else setPosition(Min);
            if(callback)
                callback->stateChanged(this);
            return;
        }
        else if ( me.getY() > bar_max )
        {
            if (Position+LargeChange < Max) setPosition(Position+LargeChange);
            else setPosition(Max);
            if(callback)
                callback->stateChanged(this);
            return;
        }
        else // clicked on the bar
        {
            bRisestate = PRESSED;
        }
    }
    
    if ( (bRisestate == PRESSED) && (me.getID() == mMouseEvent::MOUSE_EVENT_DRAGGED) )
    {
        int scroll_begin = position.y+bSize;
        int scroll_end = scroll_begin + (size.y - (bSize * 2));
        int mouse_y = me.getY();
        if ( mouse_y < scroll_begin )
        {
            mouse_y = scroll_begin;
        }
        else if ( mouse_y > scroll_end )
        {
            mouse_y = scroll_end;
        }
        
        int base_y = mouse_y - (position.y+bSize);
        int total_size = size.y-(bSize * 2);
        
        int new_pos = 0;
        if ( base_y > 0 )
        {
            new_pos = ( base_y * (Max-Min))/total_size;
        }
        
        if ( new_pos < 0 )
        {
            new_pos = 0;
        }
        else if ( new_pos > (Max-Min) )
        {
            new_pos = Max-Min;
        }
        
        if ( Position != (Min + new_pos) )
        {
            setPosition(Min + new_pos);
            if(callback) callback->stateChanged(this);
        }
    }
    
    
    if (me.getID() == mMouseEvent::MOUSE_EVENT_ENTERED
            || me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED)
    {
        bRisestate = OVER;
        dirty = true;
        return;
    }
    if (me.getID() == mMouseEvent::MOUSE_EVENT_EXITED)
    {
        bUpstate = NORMAL;
        bDownstate = NORMAL;
        bRisestate = NORMAL;
        dirty = true;
    }
}

void tVScrollBar::render()
{
    surface.fill(background);
    surface.drawRect(surface.getRect(), ctWindowsBorder);
    if (bUpstate == PRESSED)
    {
        bUpOver.bltTrans(surface, 1, 1);
    }
    else if (bUpstate == OVER)
    {
        bUpOver.bltTrans(surface, 0, 0);
    }
    else
    {
        bUpNormal.bltTrans(surface, 0, 0);
    }

    if (bDownstate == PRESSED)
    {
        bDownOver.bltTrans(surface, 1, size.y-(bSize-1));
    }
    else if (bDownstate == OVER)
    {
        bDownOver.bltTrans(surface, 0, size.y-bSize);
    } 
    else
    {
        bDownNormal.bltTrans(surface, 0, size.y-bSize);
    }
    
    if ( (bRisestate == OVER) || (bRisestate == PRESSED) )
    {
        bRiseOver.bltTrans(surface, 0,  bSize+RisePos);
    } 
    else
    {
        bRiseNormal.bltTrans(surface, 0,  bSize+RisePos);
    }
}

void tVScrollBar::setPosition(int newPosition)
{
    if (newPosition < 0) return;
    Position = newPosition;

    int R = Max - Min;
    if (R == 0)
    {
        RisePos = 0;
    }
    else
    {
        RisePos = ((Position-Min) * (size.y-(bSize*3))) / R+1; // XXX / R+1 or (R+1) it is different...
    }
    dirty = true;
    
    if(callback) callback->stateChanged(this);
}

void tVScrollBar::setMax(int newMax)
{
    Max = newMax;
    if (Max < 1) enabled = false;
    else enabled = true;
}

void tVScrollBar::setMin(int newMin)
{
    Min = newMin;
}

void tVScrollBar::setColor(PIX newColor)
{
    background = newColor;
    dirty = true;
}

void tVScrollBar::setHeight(int newHeight)
{
    setSize(bSize, newHeight);
    dirty = true;
}



