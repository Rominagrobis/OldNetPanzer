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
#ifndef __tChatBox_hpp__
#define __tChatBox_hpp__

#include "Views/Components/tVScrollBar.hpp"
#include "Views/Components/tStringListBox.hpp"

class ChatMessage
{
public:
    PIX Color;
    bool IsFlag;
    FlagID FlagIndex;
};

class tChatBox : public tStringListBox
{
public:
    tChatBox(iRect rect, StateChangedCallback* newcallback)
        : tStringListBox(rect, newcallback)
    {
        setAutoWrap(true);
    }
        
    virtual int getMaxItemWidth(const DataItem& data);
    virtual void onPaint(Surface &dst, const DataItem& data, int SubLine);
    virtual void AddChat(std::string msg, PIX color, bool isflag, FlagID flagindex);
    void deleteData(const DataItem& data)
    {
        ChatMessage *MsgData = (ChatMessage*)(data.Data);
        if ( MsgData )
        {
            delete MsgData;
        }
    }
    
    virtual void Clear()
    {
        std::list<DataItem>::iterator f;
        for (f = List.begin(); f != List.end(); f++)
        {
            ChatMessage *MsgData = (ChatMessage*)(f->Data);
            delete MsgData;
        }
        tStringListBox::Clear();
    }
    
    int getNumLines( int width, const DataItem& data);
    
};

#endif
