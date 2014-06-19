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
#ifndef __tPlayerStateBox_hpp__
#define __tPlayerStateBox_hpp__

#include "Component.hpp"
#include "MouseEvent.hpp"
#include "Views/Components/tVScrollBar.hpp"
#include "Views/Components/tStringListBox.hpp"

class tPlayerStateBox : public tStringListBox
{
private:
    bool DrawFlags;
    bool ShowTeam;
    Uint8 TeamNumber;
public:
    tPlayerStateBox(iRect rect, StateChangedCallback* newcallback);

    virtual int getMaxItemWidth(const DataItem& data);
    virtual void onPaint(Surface &dst, const DataItem& data);
    void UpdateState(bool ForceUpdate);
    void setDrawFlags(bool df);
    void setShowTeam(Uint8 Team_Number);
};

#endif 
