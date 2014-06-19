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


#include "Views/Components/tHScrollBar.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"

#define bSize (15)

void tHScrollBar::initScrollBar()
{
    Surface bitmap;
    bitmap.loadBMP(itScroll);
    bRiseOver.grab(bitmap, iRect(bSize*4, bSize, bSize*5, bSize*2));
    bRiseNormal.grab(bitmap, iRect(bSize*4, 0, bSize*5, bSize));
    
    bRightOver.grab(bitmap, iRect(bSize*2, bSize, bSize*3, bSize*2));
    bRightNormal.grab(bitmap, iRect(bSize*3, bSize, bSize*4, bSize*2));
    bLeftOver.grab(bitmap, iRect(0, bSize, bSize, bSize*2));
    bLeftNormal.grab(bitmap, iRect(bSize, bSize, bSize*2, bSize*2));

    setLocation(iXY(0,0));
    setSize(0, bSize);

    Position = 0;
    Min = 0;
    Max = 100;
    RisePos = 0;
    bRisestate = NORMAL;
    bLeftstate = NORMAL;
    bRightstate = NORMAL;
    background = ctWindowsbackground;
    SmallChange = 1;
    LargeChange = 10;
}
tHScrollBar::tHScrollBar()
{
    initScrollBar();
}

tHScrollBar::tHScrollBar(iXY pos, int Width, StateChangedCallback* newcallback)
{
    callback = newcallback;
    setLocation(pos);
    setSize(Width, bSize);
    initScrollBar();
    dirty = true;
}

void tHScrollBar::actionPerformed(const mMouseEvent &me)
{
    if (!enabled) return;
    iRect rect_button_Left(position.x, position.y, position.x+bSize, position.y+bSize);
    iRect rect_button_Right(position.x+size.x-bSize, position.y, position.x+size.x, position.y+size.y);
    
    if (rect_button_Left.contains(iXY(me.getX(), me.getY())))
    {
        bLeftstate = OVER;
        dirty = true;
        if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
        {
            bLeftstate = PRESSED;
            if (Position-SmallChange > Min) setPosition(Position-SmallChange);
            else setPosition(Min);
            if(callback)
                callback->stateChanged(this);
            return;
        }
    }
    else
    {
        bLeftstate = NORMAL;
        dirty = true;
    }

    if (rect_button_Right.contains(iXY(me.getX(), me.getY())))
    {
        bRightstate = OVER;
        dirty = true;
        if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
        {
            bRightstate = PRESSED;
            if (Position+SmallChange < Max) setPosition(Position+SmallChange);
            else setPosition(Max);
            if(callback)
                callback->stateChanged(this);
            return;
        }
    }
    else
    {
        bRightstate = NORMAL;
        dirty = true;
    }

    if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
    {
        if (me.getX() < (position.x+bSize+RisePos)) 
        {
            if (Position-LargeChange > Min) setPosition(Position-LargeChange);
            else setPosition(Min);
            if(callback)
                callback->stateChanged(this);
            return;
        }
        if (me.getX() > (position.x+bSize+RisePos+bSize))
        {
            if (Position+LargeChange < Max) setPosition(Position+LargeChange);
            else setPosition(Max);
            if(callback)
                callback->stateChanged(this);
            return;
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
        bLeftstate = NORMAL;
        bRightstate = NORMAL;
        bRisestate = NORMAL;
        dirty = true;
    }
}

void tHScrollBar::render()
{
    surface.fill(background);
    surface.drawRect(surface.getRect(), ctWindowsBorder);
    if (bLeftstate == PRESSED)
    {
        bLeftOver.bltTrans(surface, 1, 1);
    }
    else if (bLeftstate == OVER)
    {
        bLeftOver.bltTrans(surface, 0, 0);
    } 
    else
    {
        bLeftNormal.bltTrans(surface, 0, 0);
    }

    if (bRightstate == PRESSED)
    {
        bRightOver.bltTrans(surface, size.x-(bSize-1), 1);
    }
    else if (bRightstate == OVER)
    {
        bRightOver.bltTrans(surface, size.x-bSize, 0);
    } 
    else
    {
        bRightNormal.bltTrans(surface, size.x-bSize, 0);
    }
    if (bRisestate == OVER)
    {
        bRiseOver.bltTrans(surface, bSize+RisePos, 0);
    } 
    else
    {
        bRiseNormal.bltTrans(surface, bSize+RisePos, 0);
    }
}

void tHScrollBar::setPosition(int newPosition)
{
    if (newPosition < 0) return;
    Position = newPosition;
    int R = Max - Min;
    if (R == 0) RisePos = 0;
    else RisePos = ((Position-Min) * (size.x-(bSize*3))) / R+1 ;
    dirty = true;
}

void tHScrollBar::setMax(int newMax)
{
    Max = newMax;
    if (Max < 1) enabled = false;
    else enabled = true;
}

void tHScrollBar::setMin(int newMin)
{
    Min = newMin;
}

void tHScrollBar::setColor(PIX newColor)
{
    background = newColor;
    dirty = true;
}

void tHScrollBar::setWidth(int newWidth)
{
    setSize(newWidth, bSize);
    dirty = true;
}



