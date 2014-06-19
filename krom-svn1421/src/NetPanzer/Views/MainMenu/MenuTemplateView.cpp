/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue
 
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

#include "2D/Palette.hpp"
#include "2D/PackedSurface.hpp"
#include "Views/MainMenu/MenuTemplateView.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/Button.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/ViewGlobals.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/StrManager.hpp"
#include "Interfaces/PlayerGameManager.hpp"
#include "System/Sound.hpp"
#include "Classes/ScreenSurface.hpp"
#include "Particles/RadarPingParticle2D.hpp"
#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Util/Exception.hpp"
#include "Multi/ServerListView.hpp"
#include "Multi/MapSelectionView.hpp"
#include "Multi/IPAddressView.hpp"
#include "Multi/PlayerNameView.hpp"
#include "Resources/ResourceManager.hpp"

#include "Actions/Action.hpp"
#include "Actions/ActionManager.hpp"

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "testing"
#endif

Surface       MenuTemplateView::backgroundSurface;
PackedSurface MenuTemplateView::titlePackedSurface;

class ShowMainViewAction : public Action
{
public:
    ShowMainViewAction() : Action(false) {}
    
    void execute()
    {
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("MenuTemplateView", true);
        Desktop::setVisibility("MainView", true);
        ((MenuTemplateView*)Desktop::getView("MenuTemplateView"))->hidePlayButton();
    }
};

class ShowHostViewAction : public Action
{
public:
    ShowHostViewAction() : Action(false) {}
    
    void execute()
    {
        gameconfig->hostorjoin = _game_session_host;
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("MenuTemplateView", true);
        Desktop::setVisibility("HostOptionsView", true);
        Desktop::setVisibility("MapSelectionView", true);
        Desktop::setVisibility("PlayerNameView", true);
        ((MenuTemplateView*)Desktop::getView("MenuTemplateView"))->showPlayButton();
        // TODO: do doLoadTitleSurface("hostTitle");
    }
};

class ShowJoinViewAction : public Action
{
public:
    ShowJoinViewAction() : Action(false) {}
    
    void execute()
    {
        gameconfig->hostorjoin = _game_session_join;
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("MenuTemplateView", true);
        Desktop::setVisibility("PlayerNameView", true);
        Desktop::setVisibility("IPAddressView", true);
        Desktop::setVisibility("ServerListView", true);
        serverlistview->refresh();
        ((MenuTemplateView*)Desktop::getView("MenuTemplateView"))->showPlayButton();
        // TODO: do doLoadTitleSurface("joinTitle");
    }
};

class ShowOptionsViewAction : public Action
{
public:
    ShowOptionsViewAction() : Action(false) {}
    
    void execute()
    {
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("OptionsView", true);
        ((MenuTemplateView*)Desktop::getView("MenuTemplateView"))->hidePlayButton();
    }
};

class ShowHelpViewAction : public Action
{
public:
    ShowHelpViewAction() : Action(false) {}
    
    void execute()
    {
        Desktop::setVisibilityAllWindows(false);
        // TODO: should doLoadTitleSurface("helpTitle");
        Desktop::setVisibility("MenuTemplateView", true);
        Desktop::setVisibilityNoDoAnything("HelpScrollView", true);
        ((MenuTemplateView*)Desktop::getView("MenuTemplateView"))->hidePlayButton();
    }
};

class OpenResignGameViewAction : public Action
{
public:
    OpenResignGameViewAction() : Action(false) {}
    
    void execute()
    {
        Desktop::setVisibility("OptionsView", false);
        Desktop::setVisibility("AreYouSureResignView", true);
    }
};

class CloseOptionsWindowAction : public Action
{
public:
    CloseOptionsWindowAction() : Action(false) {}
    
    void execute()
    {
        GameManager::setNetPanzerGameOptions();
        Desktop::setVisibility("OptionsView", false);
    }
};

class OpenExitConfirmWindowAction : public Action
{
public:
    OpenExitConfirmWindowAction() : Action(false) {}
    
    void execute()
    {
        Desktop::setVisibility("OptionsView", false);
        Desktop::setVisibility("AreYouSureExitView", true);
    }
};

class PlayButtonClickedAction : public Action
{
public:
    PlayButtonClickedAction() : Action(false) {}
    
    void execute()
    {
        GameConfig::player_name->assign(((PlayerNameView*)Desktop::getView("PlayerNameView"))->getSelectedName());
        
        if ( GameConfig::player_name->length() == 0 )
            return;

        // Check a few things which should be ok.

        if ( ((MapSelectionView*)Desktop::getView("MapSelectionView"))->getCurrentSelectedMapNumber() == -1 )
        {
            return;
        }

        const NPString& serv_ip = ((IPAddressView*)Desktop::getView("IPAddressView"))->getSelectedServerIp();
        
        if ( (gameconfig->hostorjoin == _game_session_join) && (serv_ip.length() == 0) )
        {
            return;
        }

        // Close all menu views.
        Desktop::setVisibilityAllWindows(false);

        if ( gameconfig->hostorjoin == _game_session_join )
        {
            gameconfig->serverConnect = serv_ip;
        }

        serverlistview->endQuery();

        // TODO: do it?
    //    MenuTemplateView::backgroundSurface.free();

        ResourceManager::updateFlagData( 0,
                                         GameConfig::player_flag_data,
                                         sizeof(GameConfig::player_flag_data) );

        PlayerGameManager* manager = (PlayerGameManager*) gamemanager;
        manager->launchMultiPlayerGame();
    }
};

static Button * createMenuButton(   const NPString& label,
                                    iXY loc,
                                    bool inverted,
                                    Action * action)
{
    Surface bitmap;
    bitmap.loadBMP("pics/backgrounds/menus/buttons/default/page.bmp");
    if ( inverted )
    {
        bitmap.flipVertical();
    }
    
    Button *b = new Button();
    b->setLocation(loc);
    b->setLabel(label);
    b->setSize(Surface::getTextLength(label)+20, 25);
    b->setTextColors(Color::gray, Color::lightGray, Color::black, Color::darkGray);

    Surface bgSurface(Surface::getTextLength(label)+20, 25, 3);
    
    b->setExtraBorder();
    
    b->borders[0][0] = Color::gray;
    b->borders[0][1] = Color::black;
    b->borders[1][0] = Color::gray;
    b->borders[1][1] = Color::gray;
    b->borders[2][0] = Color::gray;
    b->borders[2][1] = Color::darkGray;
    
    iRect r = bgSurface.getRect();
    
    for ( unsigned int n = 0; n < bgSurface.getNumFrames(); n++ )
    {
        bgSurface.setFrame(n);
        bgSurface.FillRoundRect(r, 3, b->borders[n][1]);
        bitmap.bltTrans(bgSurface, 0, 0);
    }
    
    b->setImage(bgSurface);
    
    b->setAction(action);

    return b;
}

// MenuTemplateView
//---------------------------------------------------------------------------
MenuTemplateView::MenuTemplateView() : RMouseHackView()
{
    setSearchName("MenuTemplateView");
    setTitle("MenuTemplate");
    setSubTitle("");

    setAllowResize(false);
    setVisible(false);
    setAllowMove(false);
    setDisplayStatusBar(false);
    setBordered(false);
    setAlwaysOnBottom(true);

    moveTo(iXY(0, 0));
    resize(iXY(800, 600));

    initButtons();
    
    curTitleFlashTime  = 0.0f;
    titleFlashTimeHalf = 0.5;
} // end MenuTemplateView constructor

//---------------------------------------------------------------------------
// initPreGameOptionButtons
void MenuTemplateView::initPreGameOptionButtons()
{
    add( createMenuButton( _("Main"),           mainPos,    false, new ShowMainViewAction()) );
    joinPos.x = mainPos.x + Surface::getTextLength(_("Main"))+22;
    add( createMenuButton( _("Join"),           joinPos,    false, new ShowJoinViewAction()) );
    hostPos.x = joinPos.x + Surface::getTextLength(_("Join"))+22;
    add( createMenuButton( _("Host"),           hostPos,    false, new ShowHostViewAction()) );
    optionsPos.x = hostPos.x + Surface::getTextLength(_("Host"))+22;
    add( createMenuButton( _("Options"),        optionsPos, false, new ShowOptionsViewAction()) );
    helpPos.x = optionsPos.x + Surface::getTextLength(_("Options"))+22;
    add( createMenuButton( _("Help"),           helpPos,    false, new ShowHelpViewAction()) );
    exitPos.x = MenuRect.max.x - Surface::getTextLength(_("Exit netPanzer"))-25;
    add( createMenuButton( _("Exit netPanzer"), exitPos,    false, ActionManager::getAction("quit")) );
    
    playButton = createMenuButton( _("Play"), iXY(-150, 0), true, new PlayButtonClickedAction());
    add( playButton );
} // end MenuTemplateView::initPreGameOptionButtons

// initInGameOptionButtons
//---------------------------------------------------------------------------
void MenuTemplateView::initInGameOptionButtons()
{
    if(!gameconfig->quickConnect)
    {
        add( createMenuButton( _("Resign"),          resignPos, false, new OpenResignGameViewAction()) );
        add( createMenuButton( _("Exit netPanzer"),  exitPos,   false, new OpenExitConfirmWindowAction()) );
    }
    else
    {
        add( createMenuButton( _("Resign"),          exitPos,   false, new OpenResignGameViewAction()) );
    }

    add( createMenuButton( _("Close Options"), returnToGamePos, false, new CloseOptionsWindowAction()) );
} // end MenuTemplateView::initInGameOptionButtons

// initButtons
//---------------------------------------------------------------------------
void MenuTemplateView::initButtons()
{
    if (Desktop::getVisible("GameView")) {
        initInGameOptionButtons();
    } else {
        initPreGameOptionButtons();
    }
} // end MenuTemplateView::initButtons

// doDraw
//---------------------------------------------------------------------------
void MenuTemplateView::doDraw(Surface &viewArea, Surface &clientArea)
{
    //setWorldRect();
    if (Desktop::getVisible("GameView"))
    {
	// When ingame, tint the game into gray
        clientArea.BltRoundRect(getClientRect(), 10, Palette::darkGray256.getColorArray());
        clientArea.RoundRect(MenuRect, 10, Color::gray);
        clientArea.drawWindowsBorder();

    }
    else
    {        
        screen->fill(0);
		// Set the following to get does exist.
        if (backgroundSurface.getNumFrames() > 0) {
            backgroundSurface.blt(viewArea, 0, 0);
        } else {
            throw Exception("Where is the background surface?");
        }
        clientArea.BltRoundRect(MenuRect, 10, Palette::darkGray256.getColorArray());
        clientArea.RoundRect(MenuRect, 10, Color::gray);

        //titlePackedSurface.blt(clientArea, bodyTextRect.min.x, 390);
        titlePackedSurface.bltBlend(clientArea, MenuRect.min.x, MenuRect.max.y-60, Palette::colorTable6040);
    }

    static char text[] =
        "NetPanzer " PACKAGE_VERSION;
        clientArea.bltString(10, 590, text, Color::yellow);

    View::doDraw(viewArea, clientArea);
} // end doDraw

// doActivate
//---------------------------------------------------------------------------
void MenuTemplateView::doActivate()
{
    // Make the activating view active, redo this please!
    Desktop::setActiveView(searchName);

    loadBackgroundSurface();
    loadTitleSurface();
    loadNetPanzerLogo();
} // end doActivate

// loadBackgroundSurface
//---------------------------------------------------------------------------
void MenuTemplateView::loadBackgroundSurface()
{
    doLoadBackgroundSurface("pics/backgrounds/menus/menu/defaultMB.bmp");
} // end MenuTemplateView::loadBackgroundSurface

// doLoadBackgroundSurface
//---------------------------------------------------------------------------
void MenuTemplateView::doLoadBackgroundSurface(const std::string& string)
{
    backgroundSurface.loadBMP(string.c_str());
} // end MenuTemplateView::doLoadBackgroundSurface

// loadTitleSurface
//---------------------------------------------------------------------------
void MenuTemplateView::loadTitleSurface()
{
    doLoadTitleSurface("mainTitle");
} // end MenuTemplateView::loadTitleSurface

// doLoadBackgroundSurface
//---------------------------------------------------------------------------
void MenuTemplateView::doLoadTitleSurface(const std::string& string)
{
    curTitleFlashTime  = 0.0f;
    titleFlashTimeHalf = 2.5;

    std::string pakString = "pics/backgrounds/menus/menu/pak/";
    pakString += string;
    pakString += ".pak";

    titlePackedSurface.load(pakString);
} // end MenuTemplateView::doLoadTitleSurface

// doDeactivate
//---------------------------------------------------------------------------
void MenuTemplateView::doDeactivate()
{
} // end doDeactivate

//---------------------------------------------------------------------------
void MenuTemplateView::loadNetPanzerLogo()
{} // end MenuTemplateView::loadNetPanzerLogo

// processEvents
//---------------------------------------------------------------------------
void MenuTemplateView::processEvents()
{
} // end MenuTemplateView::processEvents

void
MenuTemplateView::showPlayButton()
{
    playButton->setLocation(playPos);
}

void
MenuTemplateView::hidePlayButton()
{
    playButton->setLocation(-100,-100);
}
