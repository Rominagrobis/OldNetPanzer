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


#include "Views/Components/tChatBox.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Resources/ResourceManager.hpp"

int tChatBox::getMaxItemWidth(const DataItem& data)
{
    return Surface::getTextLength(data.text)+34;
}

void tChatBox::onPaint(Surface &dst, const DataItem& data, int SubLine)
{
    int StartX = 0;
    PIX color = ctTexteNormal;
    int StartChar = 0;
    int NumPrintableChars = dst.getWidth() / 8;
    
    StartChar = SubLine * NumPrintableChars; // XXX not checking for out of bounds
    
    ChatMessage *MsgData = (ChatMessage*)(data.Data);
    if (MsgData)
    {
        color = MsgData->Color;
        
        if (MsgData->IsFlag)
        {
            if ( ! SubLine )
            {
                StartX = 24;
                Surface * flag = 0;
                flag = ResourceManager::getFlag(MsgData->FlagIndex);
                if (flag)
                {
                    flag->blt(dst, 0, 0);
                }
                
                NumPrintableChars -= 3;
            }
            else
            {
                StartChar -= 3;
            }
        } 
    }
    
    dst.bltStringLen(StartX , 4, data.text.c_str() + StartChar, NumPrintableChars, color);
}

void tChatBox::AddChat(std::string msg, PIX color, bool isflag, FlagID flagindex)
{
    ChatMessage *Datamsg;
    Datamsg = new ChatMessage();
    Datamsg->Color = color;
    Datamsg->IsFlag = isflag;
    Datamsg->FlagIndex = flagindex;

    AddData(msg, Datamsg);
}

int tChatBox::getNumLines(int width, const DataItem& data)
{
    int len = data.text.size() * 8;
    
    bool hasFlag = data.Data ? ((ChatMessage*)data.Data)->IsFlag : false;
    
    if ( hasFlag )
    {
        len += 3 * 8; // 3 more chars for the flag
    }
    
    int nlines = len / width;
    if ( len % width )
    {
        nlines += 1;
    }
    
    return nlines;
}
