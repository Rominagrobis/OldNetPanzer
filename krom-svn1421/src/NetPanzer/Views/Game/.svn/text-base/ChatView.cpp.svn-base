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

#include "Views/Game/ChatView.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Interfaces/KeyboardInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"
#include "Views/Components/Button.hpp"

#include "Actions/Action.hpp"

#include "Scripts/ScriptManager.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include "Actions/Action.hpp"

//#define CommandMax 13
//
//std::string CommandList[CommandMax] = {
//    "/server listcommands", "/server adminlogin", "/server unitspawnlist", "/server unitprofiles",
//    "/server listprofiles", "/server kick", "/server baselimit", "/server gamepass",
//    "/server map ", "/server autokick", "/server say", "/server listplayers", "/server flagtimer"}; 

//    case SDLK_TAB: {
//            cursorPos = 0;
//            strcpy(inputString, CommandList[commandPos].c_str());
//            commandPos++;
//            if (commandPos >= CommandMax) commandPos = 0;
//            cursorPos = strlen(inputString);
//        }

class SwitchChatModeAction : public Action
{
public:
    ChatView * view;
    SwitchChatModeAction(ChatView * view) : Action(false), view(view) {}
    void execute()
    {
        view->switchChatMode();
    }
};


static Button* createTitlebarButton(int x, int y, const Surface& button_images, Action* action)
{
    Surface bimage(15,15,4);
    
    bimage.setFrame(0);
    button_images.blt(bimage, 0, 0);
    bimage.bltLookup(bimage.getRect(), Palette::darkGray256.getColorArray());
    
    bimage.setFrame(1);
    button_images.blt(bimage, 0, 0);
    
    bimage.setFrame(2);
//    bimage.fill(0);
    button_images.blt(bimage, 0, 0);
    
    bimage.setFrame(3);
    button_images.blt(bimage, 0, 0);
    bimage.bltLookup(bimage.getRect(), Palette::darkGray256.getColorArray());
    
    Button *b = new Button();
    b->setImage(bimage);
    b->setLocation( x, y);
    b->setStateOffset(Button::BPRESSED, 1, 1);
    
    b->setAction(action);
    
    return b;
}

class MinimizeChatAction : public Action
{
public:
    ChatView * view;
    MinimizeChatAction(ChatView * view) : Action(false), view(view) {}
    void execute()
    {
        view->minimizeChat();
    }
};

class RetoreChatAction : public Action
{
public:
    ChatView * view;
    RetoreChatAction(ChatView * view) : Action(false), view(view) {}
    void execute()
    {
        view->restoreChat();
    }
};

ChatView::ChatView() : GameTemplateView()
{
    setSearchName("ChatView");
    setTitle("Chat View");
    setSubTitle("");

    setAllowResize(false);
    setVisible(false);
    setAllowMove(true);
    setBordered(false);
    resize(500, 150);
    moveTo(screen->getWidth()-getSizeX(), screen->getHeight()-getSizeY());

    Surface button_images;
    button_images.loadBMP(itScroll);
    
    bHideWindow = createTitlebarButton( 0, 0, button_images, new MinimizeChatAction(this));
    add(bHideWindow);
    
    button_images.setOffsetX(-30);
    bShowWindow = createTitlebarButton( 15, 0, button_images, new RetoreChatAction(this));
    bShowWindow->disable();
    add(bShowWindow);
    
    HideWindow = false;

    iRect r(0, 15, getSizeX()-15, getSizeY()-17);
    ChatList = new tChatBox(r, 0);
    vsbChat = new tVScrollBar();
    add(vsbChat);
    vsbChat->setLargeChange(ChatList->getNumVisibleLines() -1);
    ChatList->setVscrollBar(vsbChat);
    ChatList->setColor(0);
    ChatList->setAutoScroll(true);
    add(ChatList);
    
    input = new InputField();
    input->setLocation(2 + 16,getSizeY()-16);
    input->setSize(getSizeX()-(5 + 17),16);
    input->setMaxTextLength(500);
    input->setExcludedChars("\\�`�");
    
    add(input);
    
    switchModeButton = new Button(); //Button::createTextButton(">", iXY(0, getSizeY()-16), 14, 0);
    
    switchModeButton->setLocation(1, getSizeY()-16);
    switchModeButton->setSize(16, 16);
    switchModeButton->setLabel(">");
    switchModeButton->setStateOffset(Button::BPRESSED, 1, 1);
    switchModeButton->setAction(new SwitchChatModeAction(this));
    
    switchModeButton->setTextColors(Color::gray, Color::yellow, Color::gray, Color::darkGray);
    
    allBg.create(16,16,1);
    allBg.FillRoundRect(allBg.getRect(), 3, Color::white);
    allBg.RoundRect(allBg.getRect(), 3, Color::white);
    
    friendsBg.create(16,16,1);
    friendsBg.FillRoundRect(friendsBg.getRect(), 3, Color::yellow);
    friendsBg.RoundRect(friendsBg.getRect(), 3, Color::yellow);

    add(switchModeButton);
    
    isAllMode = true;
    setChatFriends();
}

void ChatView::doDraw(Surface &viewArea, Surface &clientArea)
{
    clientArea.bltLookup(viewArea.getRect(), Palette::brightness256.getColorArray());
    clientArea.bltString(35, 4, title, ctTexteNormal);
    View::doDraw(viewArea, clientArea);
}

void ChatView::processEvents()
{
    if ( Desktop::getKeyboardFocusComponent() == input )
    {
        if ( (KeyboardInterface::getKeyPressed(SDLK_RETURN) || KeyboardInterface::getKeyPressed(SDLK_KP_ENTER) ) )
        {
            if ( input->getText().length() != 0 )
            {
                const NPString& msg = input->getText();

                if ( msg[0] != '/' || ! ScriptManager::runUserCommand( msg.substr(1) ) )
                {
                    if ( isAllMode )
                    {
                        ChatInterface::say(msg);
                    }
                    else
                    {
                        ChatInterface::teamsay(msg);
                    }
                }

                input->setText("");
            }
        }
    }
#ifdef WIN32
    else if ( (KeyboardInterface::getKeyState(SDLK_LCTRL) || KeyboardInterface::getKeyState(SDLK_RCTRL))
                  && (KeyboardInterface::getKeyPressed(SDLK_c) ))
    {
        std::string str = ChatList->getTextItem();
        if (str != "")
        {
            if(OpenClipboard(NULL))
            {
                int pos = str.find_first_of(':')+2;
                std::string chaine = str.substr(pos);
                HGLOBAL hText = GlobalAlloc(GMEM_DDESHARE, chaine.length()+1);
                char * pText = (char*)GlobalLock(hText);
                strcpy(pText, chaine.c_str());
                GlobalUnlock(hText);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, hText);
                CloseClipboard();
            }
        }
    }
#endif
    else
    {
        COMMAND_PROCESSOR.process(false);
    }
}

void ChatView::minimizeChat()
{
    HideWindow = true;
    bHideWindow->disable();
    bShowWindow->enable();
    
    if ( Desktop::getKeyboardFocusComponent() == input )
    {
        Desktop::setKeyboardFocusComponent(0);
    }
    
    removeComponent(input);
    removeComponent(switchModeButton);

    resize(500, 17);
    moveTo(screen->getWidth()-getSizeX(), screen->getHeight()-getSizeY());
}

void ChatView::restoreChat()
{
    HideWindow = false;
    bHideWindow->enable();
    bShowWindow->disable();

    resize(500, 150);
    moveTo(screen->getWidth()-getSizeX(), screen->getHeight()-getSizeY());
    
    add(switchModeButton);
    add(input);
}

void ChatView::postMessage(PIX msgcolor, bool hasFlag, FlagID flag, const char *format, ...)
{
    char temp_str[256];
    va_list vap;

    va_start( vap, format );
    vsnprintf( temp_str, 256, format, vap );
    va_end( vap );

    ChatList->AddChat(temp_str, msgcolor, hasFlag, flag);
}

void ChatView::checkResolution(iXY oldResolution, iXY newResolution)
{
    moveTo(screen->getWidth()-getSizeX(), screen->getHeight()-getSizeY());
}

void ChatView::clear()
{
    ChatList->Clear();
}

void ChatView::setChatFriends()
{
    if ( isAllMode )
    {
        isAllMode = false;
        switchModeButton->setImage(friendsBg);
        input->setTextColor(Color::yellow);
    }
}

void ChatView::setChatAll()
{
    if ( ! isAllMode )
    {
        isAllMode = true;
        switchModeButton->setImage(allBg);
        input->setTextColor(Color::white);
    }
}

void ChatView::openChat()
{
    if ( HideWindow )
    {
        restoreChat();
    }
    
    Desktop::setKeyboardFocusComponent(input);
}

void ChatView::openFriendsChat()
{
    if ( HideWindow )
    {
        restoreChat();
    }
    
    setChatFriends();
    
    Desktop::setKeyboardFocusComponent(input);
}

void ChatView::switchChatMode()
{
    if ( isAllMode )
    {
        setChatFriends();
    }
    else
    {
        setChatAll();
    }
}
