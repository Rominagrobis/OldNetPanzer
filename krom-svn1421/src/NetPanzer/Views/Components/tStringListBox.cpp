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


#include "Views/Components/tStringListBox.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"

#define MAX_CHAT_LINES (2000)

tStringListBox::tStringListBox(iRect rect, StateChangedCallback* newcallback)
{
    VScrollBar = 0;
    HScrollBar = 0;
    callback = newcallback;
    setLocation(rect.min);
    setSize(rect.getSizeX(), rect.getSizeY());
    background = ctWindowsbackground;
    StartItem = List.end();
    StartSubLine = 0;
    StartWidth = 0;
    TotalLines = 0;
    TotalPosition = 0;
    MaxItemWidth = size.x;
    SelectedItem = List.end();
    MaxItemView = (size.y-5)/ItemHeight;
    AutoScroll = false;
    AutoWrap = false;
    dirty = true;
}

void tStringListBox::actionPerformed(const mMouseEvent &me)
{
    if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED &&
        (me.getModifiers() & InputEvent::BUTTON1_MASK)) 
    {
        int SelectedLine = (me.getY()-(position.y+4))/ItemHeight;
        SelectedItem = StartItem;
        int TmpSubLine = StartSubLine;
        
        SelectedItem = StartItem;
        
        while ( SelectedLine-- && SelectedItem != List.end() )
        {
            if ( ++TmpSubLine >= (*SelectedItem).num_lines )
            {
                SelectedItem++;
                TmpSubLine = 0;
            }
        }
        dirty = true;
    }
}

std::string tStringListBox::getTextItem()
{
    if ( SelectedItem == List.end() )
    {
        return "";
    }
    return (*SelectedItem).text;
}

int tStringListBox::getMaxItemWidth(const DataItem& data)
{
    return Surface::getTextLength(data.text)+10;
}

void tStringListBox::AddData(const std::string& S, void * D)
{
    int DisplaySize = size.x - 6;
    
    DataItem data;
    data.text = S;
    data.Data = D;
    data.num_lines = getNumLines(DisplaySize, data);
    List.push_back(data);

    if ( List.size() > MAX_CHAT_LINES )
    {
        bool was_first = StartItem == List.begin();
        bool was_selected = SelectedItem == List.begin();
        
        const DataItem& d = (*List.begin());
        
        TotalLines -= d.num_lines;
        TotalPosition = std::max(0, TotalPosition - d.num_lines);
        
        deleteData(d);
        
        List.pop_front();
        
        if ( was_first )
        {
            StartItem = List.begin();
        }
        
        if ( was_selected )
        {
            SelectedItem = List.end();
        }
    }
    
    if ( StartItem == List.end() )
    {
        StartItem = List.begin();
    }
    
    TotalLines += data.num_lines;
    
    int HSize = getMaxItemWidth(data);
    
    if (MaxItemWidth < HSize)
    {
        MaxItemWidth = HSize;
    }
    
    if (VScrollBar)
    {
        VScrollBar->setMax(TotalLines-MaxItemView);
    }
    
    if (HScrollBar)
    {
        HScrollBar->setMax(MaxItemWidth-size.x);
    }
    
    if (AutoScroll && VScrollBar) 
    {
        VScrollBar->setPosition(VScrollBar->getMax());
        
        int new_pos = VScrollBar->getPosition();
        
        if ( new_pos < TotalPosition )
        {
            while ( TotalPosition > new_pos )
            {
                if ( (--StartSubLine) < 0 )
                {
                    StartItem--;
                    StartSubLine = (*StartItem).num_lines - 1;
                }
                TotalPosition -= 1;
            }
        }
        else if ( new_pos > TotalPosition )
        {
            while ( TotalPosition < new_pos )
            {
                if ( (++StartSubLine) >= (*StartItem).num_lines )
                {
                    StartItem++;
                    StartSubLine = 0;
                }
                TotalPosition += 1;
            }
        }
    }
    
    dirty = true;
}

void tStringListBox::render()
{
    surface.fill(background);
    surface.drawRect(surface.getRect(), ctWindowsBorder);
    int row = 4;
    
    int maxw = 0;
    if ( AutoWrap )
    {
        maxw = size.x - 6;
    }
    else
    {
        maxw = MaxItemWidth+10;
    }
    
    Surface RowPaint(maxw, ItemHeight, 1);
    Surface Bitmap(size.x-6, ItemHeight, 1);
    
    std::list<DataItem>::iterator item = StartItem;
    int subline_num = StartSubLine;
    int max_size_y = size.y - ItemHeight;
    
    while ( (row < max_size_y) && (item != List.end()) )
    {
        RowPaint.fill(background);
        if ( SelectedItem == item )
        {
            RowPaint.fill(ctTexteOver);
        }
        
        onPaint(RowPaint, *item, subline_num);
        
        iRect r(StartWidth, 0, StartWidth+(size.x-6), ItemHeight);
        Bitmap.grab(RowPaint, r);
        Bitmap.blt(surface, 3, row);
        
        row += ItemHeight;
        subline_num += 1;
        if ( subline_num >= (*item).num_lines )
        {
            subline_num = 0;
            item++;
        }
    }
}

void tStringListBox::onPaint(Surface &dst, const DataItem& data, int SubLine)
{
    dst.bltString(1 , 4, data.text.c_str(), ctTexteNormal);
}

void tStringListBox::setColor(PIX newColor)
{
    background = newColor;
    dirty = true;
}

void tStringListBox::setVscrollBar(tVScrollBar *newVScrollBar)
{
    VScrollBar = newVScrollBar;
    if (VScrollBar)
    {
        VScrollBar->setLocation(position.x+size.x, position.y);
        VScrollBar->setHeight(size.y);
        VScrollBar->setStateChangedCallback(this);
        VScrollBar->setMax(List.size()-MaxItemView);
    }
}

void tStringListBox::setHscrollBar(tHScrollBar *newHScrollBar)
{
    HScrollBar = newHScrollBar;
    if (HScrollBar)
    {
        HScrollBar->setLocation(position.x, position.y+size.y);
        HScrollBar->setWidth(size.x);
        HScrollBar->setStateChangedCallback(this);
        HScrollBar->setSmallChange(3);
        HScrollBar->setMax(MaxItemWidth-size.x);
    }
}

void tStringListBox::stateChanged(Component* source)
{
    if (source == VScrollBar)
    {
        int new_pos = VScrollBar->getPosition();
        
        if ( new_pos < TotalPosition )
        {
            while ( TotalPosition > new_pos )
            {
                if ( (--StartSubLine) < 0 )
                {
                    StartItem--;
                    StartSubLine = (*StartItem).num_lines - 1;
                }
                TotalPosition -= 1;
            }
        }
        else if ( new_pos > TotalPosition )
        {
            while ( TotalPosition < new_pos )
            {
                if ( (++StartSubLine) >= (*StartItem).num_lines )
                {
                    StartItem++;
                    StartSubLine = 0;
                }
                TotalPosition += 1;
            }
        }

        dirty = true;
    }
    if (source == HScrollBar)
    {
        StartWidth = HScrollBar->getPosition();
        dirty = true;
    }
}

void tStringListBox::setLocation(int x, int y)
{
    position.x = x;
    position.y = y;
    if (VScrollBar)
    {
        VScrollBar->setLocation(position.x+size.x, position.y);
    }
    if (HScrollBar)
    {
        HScrollBar->setLocation(position.x, position.y+size.y);
    }
}

void tStringListBox::setAutoWrap(bool autowrap)
{
    if ( this->AutoWrap != autowrap )
    {
        this->AutoWrap = autowrap;
        StartSubLine = 0;
        if ( autowrap )
        {
            int NumLines = 0;
            int DisplaySize = size.x - 6;
            int c;
            
            std::list<DataItem>::iterator i = List.begin();
            while ( i != List.end() )
            {
                c = getNumLines( DisplaySize, *i);
                NumLines += c;
                (*i).num_lines = c;
                i++;
            }
            
            TotalLines = NumLines;
        }
        else
        {
            TotalLines = List.size();
        }
    }
}
