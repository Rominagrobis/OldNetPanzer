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

#include "HelpScrollView.hpp"
#include "GameView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/Desktop.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Interfaces/StrManager.hpp"

void HelpScrollView::addhelpcmd(NPString cmd, NPString help)
{
    char helpBuf[256];
    snprintf(helpBuf, sizeof(helpBuf), "  %-30s %s", cmd.c_str(), help.c_str());
    HelpBox->Add(helpBuf);
}

HelpScrollView::HelpScrollView() : SpecialButtonView()
{
    setSearchName("HelpScrollView");
    setTitle(_("Help Information"));
    setSubTitle("");
    setAllowResize(false);
    setAllowMove(false);
    
    moveTo(bodyTextRect.min);
    resize(bodyTextRect.getSize());
    iRect r(0, 0, bodyTextRect.getSizeX()-20, bodyTextRect.getSizeY()-33);
    HelpBox = new tStringListBox(r, 0);
    VscrollBar = new tVScrollBar();
    add(VscrollBar);
    HscrollBar = new tHScrollBar();
    add(HscrollBar);
    HelpBox->setVscrollBar(VscrollBar);
    HelpBox->setHscrollBar(HscrollBar);
    add(HelpBox);

    HelpBox->Add(_("Key"));
    HelpBox->Add("");
    addhelpcmd(_("LMC ="), _("Left Mouse Click"));
    addhelpcmd(_("RMC ="), _("Right Mouse Click"));
    addhelpcmd(_("RMH ="), _("Right Mouse Hold"));
    addhelpcmd(_("LMH ="), _("Left Mouse Hold"));
    HelpBox->Add("");
    HelpBox->Add("");
    HelpBox->Add(_("Unit Related"));
    HelpBox->Add("");
    addhelpcmd(_("LMC"), _("Selects a unit"));
    addhelpcmd(_("LMH"), _("Makes a bouding box for unit selection"));
    addhelpcmd(_("RMC"), _("De-selects all selected units"));
    addhelpcmd(_("LMC + Ctrl"), _("Directs (force/manual) fire of a unit"));
    addhelpcmd(_("LMC + Shift, LMC + Ctrl"), _("Add unit to selection"));
    addhelpcmd(_("Ctrl + [0..9]"), _("Define a group of units"));
    addhelpcmd(_("[0..9]"), _("Recall a group of units"));
    addhelpcmd(_("[0..9] (2 times)"), _("Center view on a group of units"));
    addhelpcmd(_("RMH"), _("Fast map scroll"));
    HelpBox->Add("");
    HelpBox->Add("");
    HelpBox->Add(_("Game Related"));
    HelpBox->Add("");
    addhelpcmd(_("LMC + 'A'"), _("Allies with the clicked unit's team"));
    addhelpcmd(_("ESC"), _("In game menus"));
    HelpBox->Add("");
    HelpBox->Add("");
    HelpBox->Add(_("Outpost Related"));
    HelpBox->Add("");
    addhelpcmd(_("'O'"), _("Toggle outpost names"));
    addhelpcmd(_("LMC on captured outpost"), _("Displays the outpost view"));
    addhelpcmd(_("mouse drag on captured outpost"), _("Select unit spawn point"));
    HelpBox->Add("");
    HelpBox->Add("");
    HelpBox->Add(_("Chat Related"));
    HelpBox->Add("");
    addhelpcmd(_("Enter"), _("Send message to all"));
    addhelpcmd(_("Ctrl + 'A'"), _("Send message to allies"));
    addhelpcmd(_("'C'"), _("Send count down message"));
    HelpBox->Add("");
    HelpBox->Add("");
    HelpBox->Add(_("View Related"));
    HelpBox->Add("");
    addhelpcmd(_("F1"), _("Help View"));
    addhelpcmd(_("'B'"), _("Toggle flag selection window"));
    addhelpcmd(_("'D'"), _("Damage View"));
    addhelpcmd(_("'F'"), _("Flag View"));
    addhelpcmd(_("'M'"), _("Mini Map View"));
    addhelpcmd(_("'N'"), _("Name View"));
    addhelpcmd(_("TAB"), _("Rank View"));
    addhelpcmd(_("RMH + mouse movement"), _("Moves any view on the screen"));
    addhelpcmd(_("RMH + [1 - 5]"), _("Sets the background drawing mode"));
    HelpBox->Add(" ");
    HelpBox->Add(" ");
    HelpBox->Add(_("Mini Map View Related"));
    HelpBox->Add("");
    addhelpcmd(_("RMH + [1 - 7]"), _("Sets the blending level of the minimap"));
    addhelpcmd(_("RMH + [+ or -]"), _("Scales the size of the minimap"));
    HelpBox->Add(" ");
    HelpBox->Add(" ");
    HelpBox->Add(_("System Related"));
    HelpBox->Add("");
    addhelpcmd(_("ALT + Enter"), _("Toggle Fullscreen/Windowed mode"));
    addhelpcmd(_("F9"), _("BMP screen shot"));
    HelpBox->Add(" ");
    HelpBox->Add(" ");
    HelpBox->Add(_("Scripting"));
//    HelpBox->Add("");
//    HelpBox->Add("");
//    HelpBox->Add(_("Client Related"));
//    HelpBox->Add("");
//    HelpBox->Add(_("  /listcommands                     List of available commands"));
//    HelpBox->Add(_("  /countdown [timeout in seconds]   Generate a countdown"));
//    HelpBox->Add(_("  /quit                             Quit the game"));
//    HelpBox->Add(_("  /help                             Show Help window (same 'F1')"));
//    HelpBox->Add(_("  /say [your talk]                  Send message to all (same 'Enter'"));
//    HelpBox->Add(_("  /teamsay [your talk]              Send message to allies (same Ctrl + 'A')"));
    HelpBox->Add("");
    HelpBox->Add(_("Server Related"));
    HelpBox->Add("");
    addhelpcmd("/server listcommands", _("List of Gameserver available commands"));
    addhelpcmd(_("/server adminlogin [password]"), _("Gameserver admin login"));
    addhelpcmd(_("/server unitspawnlist [list]"), _("List/set spawn units (number)"));
    addhelpcmd(_("/server unitprofiles [list]"), _("List/set spawn units (types)"));
    addhelpcmd(_("/server map [map name]"), _("Restart game with the given map"));
    addhelpcmd(_("/server autokick [timeout]"), _("Show/set autokick for inactive players"));

    addhelpcmd(_("/server flagtimer [timeout]"), _("Show/set flag change timer"));
    addhelpcmd(_("/server say [your talk]"), _("Server say something in chat"));
    addhelpcmd("/server listplayers", _("List players with the assigned number"));
    addhelpcmd("/server listprofiles", _("List available profiles"));
    addhelpcmd(_("/server kick [player number]"), _("Kick a given player"));
    addhelpcmd(_("/server baselimit [max bases]"), _("Show/set the max bases number per player"));
    addhelpcmd(_("/server gamepass [password]"), _("Show/set game connection password"));
    HelpBox->Add(" ");
}

void HelpScrollView::doDraw(Surface &viewArea, Surface &clientArea)
{
    if (Desktop::getVisible("GameView")) {
        bltViewBackground(viewArea);
    }

    View::doDraw(viewArea, clientArea);
} // end HelpScrollView::doDraw

void HelpScrollView::doActivate()
{
    /* empty */
}

void HelpScrollView::processEvents()
{
    if ( Desktop::getVisible("GameView") )
        COMMAND_PROCESSOR.process(false);
}

