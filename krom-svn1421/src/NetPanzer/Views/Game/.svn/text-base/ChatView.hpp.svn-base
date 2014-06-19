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

#ifndef __CHATVIEW_HPP__
#define __CHATVIEW_HPP__

#include "Views/Components/tChatBox.hpp"
#include "Views/Components/InputField.hpp"
#include "Views/Components/tVScrollBar.hpp"
#include "Views/Components/tHScrollBar.hpp"
#include "Views/Game/GameTemplateView.hpp"

class Button;

class ChatView : public GameTemplateView
{
private:

    tChatBox * ChatList;
    tVScrollBar * vsbChat;
    Button * bHideWindow;
    Button * bShowWindow;
    Button * switchModeButton;
    InputField * input;
    bool HideWindow;
    
    bool isAllMode;
    Surface allBg;
    Surface friendsBg;
    
public:
    ChatView();
    virtual ~ChatView()
    {}

    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void processEvents();
    virtual void checkResolution(iXY oldResolution, iXY newResolution);
    void postMessage( PIX msgColor, bool hasFlag, FlagID flag, const char *format, ... );
    void clear();
    
    void minimizeChat();
    void restoreChat();
    
    void setChatFriends();
    void setChatAll();
    
    void openChat();
    void openFriendsChat();
    
    void switchChatMode();
    
    tChatBox * getChatBox() { return ChatList; }
    
};

#endif
