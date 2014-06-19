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
 * Created on September 4, 2012, 9:36 PM
 */

#ifndef DISCONNECTACTION_HPP
#define	DISCONNECTACTION_HPP

#include "Action.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"
#include "Views/MainMenu/OptionsTemplateView.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/StrManager.hpp"
#include "2D/Color.hpp"


class DisconnectAction : public Action
{
public:
    DisconnectAction() : Action(true) {}
    void execute()
    {
        MenuTemplateView * menuview = (MenuTemplateView*)Desktop::getView("MenuTemplateView");
        OptionsTemplateView * optionsview  = (OptionsTemplateView*)Desktop::getView("OptionsView");
        
        GameManager::drawTextCenteredOnScreen(_("Loading Main View..."), Color::white);

        GameManager::quitNetPanzerGame();

        GameManager::drawTextCenteredOnScreen(_("Loading Main View..."), Color::white);

        if ( menuview )
        {
            menuview->hidePlayButton();
        }
        
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("MenuTemplateView", true);
        Desktop::setVisibility("MainView", true);

        if ( optionsview )
        {
            optionsview->initButtons();
            optionsview->setAlwaysOnBottom(true);
        }
    }
};


#endif	/* DISCONNECTACTION_HPP */

