/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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


#include "ServerListView.hpp"

#include <iostream>
#include <sstream>
#include "IPAddressView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "MasterServer/ServerInfo.hpp"
#include "Core/NetworkGlobals.hpp"
// XXX ultrahack
#include "Classes/ScreenSurface.hpp"
#include "Views/Components/Desktop.hpp"
#include "Interfaces/StrManager.hpp"
#include "Views/Game/LoadingView.hpp"
#include "Actions/Action.hpp"

ServerListView* serverlistview = 0;

class RefreshServerListAction : public Action
{
public:
    RefreshServerListAction() : Action(false) {};
    
    void execute()
    {
        serverlistview->refresh();
    }
};
ServerListView::ServerListView()
    : queryThread(0)
{
    char str_buf[256];
    setSearchName("ServerListView");
    setTitle(_("Servers"));
    sprintf( str_buf, " %-25s %-14s %-12s %10s", _("Name"), _("Players/max"), _("Map"), _("Ping") );
    setSubTitle(str_buf);

    moveTo(bodyTextRect.min + iXY(0, 205));
    resizeClientArea(bodyTextRect.max - bodyTextRect.min - iXY(5,220));

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    add( Button::createTextButton( _("Refresh"),
                                   iXY(getClientRect().getSizeX()-82, getClientRect().getSizeY() - Surface::getFontHeight() * 2),
                                   80,  new RefreshServerListAction()));
    
    // XXX ugly
    serverlistview = this;

    lock_image.loadBMP("pics/default/lock.bmp");
}

ServerListView::~ServerListView()
{
    delete queryThread;
    serverlistview = 0;
}

void
ServerListView::refresh()
{
    if ( queryThread ) { 
        if (queryThread->isRunning())
            return;
        else
            delete queryThread;
    }
    queryThread = 0;
    
    // don't clear before the delete or after the new, as the thread contains
    // pointers to the serverlist
    for(std::vector<masterserver::ServerInfo*>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i)
        delete *i;
    serverlist.clear();

    bool mylock = false;
    if ( ! screen->getDoesExist() ) {
        screen->lock();
        mylock = true;
    }
    Desktop::draw(*screen); // XXX ultrahack
    if ( mylock )
        screen->unlock();
    screen->copyToVideoFlip(); // XXX uberhack

    queryThread = new masterserver::ServerQueryThread(&serverlist);   
}

void
ServerListView::endQuery()
{
    if (queryThread) {
        delete queryThread;
        queryThread=0;
    }
}

void
ServerListView::doDraw(Surface& windowArea, Surface& clientArea)
{
    clientArea.fill(Color::black);
    
    if(queryThread && queryThread->isRunning()) {
        queryThread->checkTimeOuts();
    }
    
    if(serverlist.empty()) {
        const char* msg;
        if ( queryThread ) {
            msg = queryThread->getStateMessage();
        } else {
            msg = _("Resolving masterserver address");
        }
        clientArea.bltString(0, 0, msg, Color::white);
        View::doDraw(windowArea, clientArea);
        return;
    }

    unsigned int y = 0;
    for(std::vector<masterserver::ServerInfo*>::iterator i = serverlist.begin();
            i != serverlist.end(); ++i) {
        const masterserver::ServerInfo& server = *(*i);

        if(server.status == masterserver::ServerInfo::QUERYING) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, _("querying..."), Color::gray);
        } else if(server.status == masterserver::ServerInfo::TIMEOUT) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, _("timeout"), Color::gray);
        } else if(server.protocol != NETPANZER_PROTOCOL_VERSION) {
            clientArea.bltString(0,   y, server.address.c_str(), Color::gray);
            clientArea.bltString(140, y, getNetpanzerProtocolMessage(server.protocol), Color::gray);
        } else {
            std::stringstream playerstr;
            playerstr << server.players << "/" << server.maxplayers;

            std::stringstream pingstr;
            pingstr << server.ping;
            
            std::stringstream servaddr;
            servaddr << server.address << ':' << server.port;
            
            Uint8 textcolor = Color::white;
            
            if (servaddr.str()==((IPAddressView*)Desktop::getView("IPAddressView"))->getSelectedServerIp()) {
                textcolor = Color::yellow;
                clientArea.fillRect(
                    iRect(0,y,clientArea.getWidth(),y+Surface::getFontHeight()),
                    Color::blue);
            }

            char ssn[44];
            SDL_strlcpy(ssn, server.name.c_str(), sizeof(ssn));
            if ( server.needs_password )
            {
                lock_image.blt(clientArea, 0, y);
            }
            clientArea.bltString(8,   y, ssn, textcolor);
            clientArea.bltString(350, y, playerstr.str().c_str(), textcolor);
            clientArea.bltString(400, y, server.map.c_str(), textcolor);
            clientArea.bltString(550, y, pingstr.str().c_str(), textcolor);

        }

        y += Surface::getFontHeight();
        if(y >= clientArea.getHeight())
            break;                             
    }

    View::doDraw(windowArea, clientArea);
}

int
ServerListView::lMouseUp(const iXY& down_pos, const iXY& up_pos)
{
    if(down_pos.x < 0 || down_pos.y < 0 || up_pos.x < 0 || up_pos.y < 0)
        return View::lMouseUp(down_pos, up_pos);

    int listpos = down_pos.y / Surface::getFontHeight();
    if(listpos >= int(serverlist.size()) || 
            serverlist[listpos]->status != masterserver::ServerInfo::RUNNING)
        return View::lMouseUp(down_pos, up_pos);

    const masterserver::ServerInfo& server = *(serverlist[listpos]);
    std::stringstream addr;
    addr << server.address << ':' << server.port;
    ((IPAddressView*)Desktop::getView("IPAddressView"))->setSelectedServerIp(addr.str());

    LoadingView * lv = static_cast<LoadingView*>(Desktop::getView("LoadingView"));
    if ( lv )
    {
        lv->setNeedPassword(server.needs_password);
    }
    
    return View::lMouseUp(down_pos, up_pos);
}

