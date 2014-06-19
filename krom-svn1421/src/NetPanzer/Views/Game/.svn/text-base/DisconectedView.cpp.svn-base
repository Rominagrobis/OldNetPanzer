/*
Copyright (C) 2007 by Aaron Perez <aaronps@gmail.com>

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

#include "DisconectedView.hpp"

#include "Interfaces/GameManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/MainMenu/OptionsTemplateView.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Util/Log.hpp"

#include "Views/Components/Button.hpp"
#include "Actions/ActionManager.hpp"

DisconectedView::DisconectedView() : SpecialButtonView()
{
    setSearchName("DisconectedView");
    setTitle("Disconected from server");
    setSubTitle("");
}

void
DisconectedView::init()
{
    setBordered(true);
    setAllowResize(false);

    resize(screen->getWidth(),screen->getHeight());
    moveTo(0,0);

    int bsize = Surface::getTextLength(" ") * 8;
    
    add( Button::createTextButton("Ok", iXY((getClientRect().getSizeX()/2)-(bsize/2), (getClientRect().getSizeY()/2)+(Surface::getFontHeight() * 2)),
                                  bsize, ActionManager::getAction("disconnect")));
}

void
DisconectedView::doDraw(Surface &viewArea, Surface &clientArea)
{
    iRect r(min, max);

    viewArea.bltLookup(r, Palette::darkGray256.getColorArray());
    viewArea.bltStringCenter(disconectMsg.c_str(), Color::white);

    View::doDraw(viewArea, clientArea);
}

void
DisconectedView::doActivate()
{
    init();
    Desktop::setActiveView(this);
}
