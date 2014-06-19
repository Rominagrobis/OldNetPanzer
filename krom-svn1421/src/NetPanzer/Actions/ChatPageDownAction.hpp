/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Aaron Perez

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
 *
 * Created on September 5, 2012, 11:58 PM
 */

#ifndef CHATPAGEDOWNACTION_HPP
#define	CHATPAGEDOWNACTION_HPP

#include "Action.hpp"
#include "Views/Game/ChatView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/tChatBox.hpp"
#include "Views/Components/tVScrollBar.hpp"

#include <algorithm>

class ChatPageDownAction : public Action
{
public:
    ChatPageDownAction() : Action(true) {}
    void execute()
    {
        ChatView *v = (ChatView*)Desktop::getView("ChatView");
        if ( v )
        {
            tChatBox * cbox = v->getChatBox();
            if ( ! cbox )
            {
                return;
            }
            
            tVScrollBar * sbar = cbox->getVscrollBar();
            if ( ! sbar )
            {
                return;
            }

            int new_pos = std::min(sbar->getMax(), sbar->getPosition() + cbox->getNumVisibleLines());
            sbar->setPosition(new_pos);
        }
    }
};

#endif	/* CHATPAGEDOWNACTION_HPP */

