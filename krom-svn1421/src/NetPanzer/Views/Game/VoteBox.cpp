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

#include "Classes/ScreenSurface.hpp"
#include "Views/Game/VoteBox.hpp"
#include "Views/Components/Button.hpp"
#include "Views/Components/Label.hpp"
#include "Views/Theme.hpp"
#include "Interfaces/VoteManager.hpp"
#include "2D/Surface.hpp"
#include "Actions/Action.hpp"

class YesNoAction : public Action
{
private:
    Uint8 result;
public:
//    VoteBox * view;
    YesNoAction(Uint8 value) : Action(false){result = value;}
    void execute()
    {
        VoteManager::playerVote(result);
    }
};

VoteBox::VoteBox(NPString msg) : GameTemplateView()
{
    int sizex = Surface::getTextLength(msg)+20;
    
    if (sizex < 200) sizex = 200;
    
    moveTo(iXY((screen->getWidth()/2)-(sizex/2), screen->getHeight()/2-150));
    resize(iXY(sizex, 75));
    
    setSearchName("votebox");
    setTitle("Message");
    setSubTitle("");
    
    setAllowResize(false);
    setAllowMove(true);
    setVisible(false);
    setBordered(false);
    
    add( new Label(10, 12, msg, Color::cyan));
    
    add(Button::createNewSpecialButton("Yes",
                                       iXY(sizex/2-55, 35),
                                       50,
                                       new YesNoAction(vote_yes)));
    
    add(Button::createNewSpecialButton("No",
                                       iXY(sizex/2+5, 35),
                                       50,
                                       new YesNoAction(vote_no)));
    
    LOGGER.warning("VoteBox %s", msg.c_str());
}

void VoteBox::doDraw(Surface &viewArea, Surface &clientArea)
{
    clientArea.FillRoundRect(getClientRect(), 8, ctWindowsbackground);
    clientArea.RoundRect(getClientRect(), 8, ctWindowsBorder);

    char statBuf[256];
    snprintf(statBuf, sizeof(statBuf),
             "Time left %d", VoteManager::getTimer());
    clientArea.bltString(getClientRect().getSizeX()/2-50, getClientRect().getSizeY()-12,statBuf, Color::yellow);

    View::doDraw(viewArea, clientArea);
    
    if (VoteManager::checkVoteTimer())
        VoteManager::playerVote(vote_nothing);
}
