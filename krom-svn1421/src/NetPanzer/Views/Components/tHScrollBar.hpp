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
#ifndef __tHScrollBar_hpp__
#define __tHScrollBar_hpp__

#include "Component.hpp"
#include "MouseEvent.hpp"
#include "Views/Components/View.hpp"
#include "Views/Components/StateChangedCallback.hpp"

class StateChangedCallback;

class tHScrollBar : public Component
{
private:
    enum BState
    {
        NORMAL =   0,
        OVER   =   1,
        PRESSED=   2,
    } ;
    StateChangedCallback* callback;
    
    BState bLeftstate;
    Surface bLeftOver;
    Surface bLeftNormal;
    
    BState bRightstate;
    Surface bRightOver;
    Surface bRightNormal;
    
    BState bRisestate;
    Surface bRiseOver;
    Surface bRiseNormal;
    
    int RisePos;
    int Position;
    int Min;
    int Max;
    int LargeChange;
    int SmallChange;
    
    virtual void render();
    virtual void actionPerformed(const mMouseEvent &me);
    void initScrollBar();
public:

    tHScrollBar();
    tHScrollBar(iXY pos, int Width, StateChangedCallback* newcallback = 0);
    virtual ~tHScrollBar()
    {}

    int getMin() {  return Min;}
    int getMax() {return Max;}
    int getPosition() {return Position;}
    void setMin(int newMin);
    void setMax(int newMax);
    void setPosition(int newPosition);
    void setColor(PIX newColor);
    void setWidth(int newWidth);
    void setSmallChange(int newSmallChange) {SmallChange =newSmallChange;}
    void setLargeChange(int newLargeChange) {LargeChange = newLargeChange;}
    void setStateChangedCallback(StateChangedCallback* newcallback)
    {
        callback = newcallback;
    }
}; 

#endif 
