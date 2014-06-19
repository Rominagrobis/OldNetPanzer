/*
Copyright (C) 2012 by Aaron Perez <aaronps@gmail.com>

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

#include "NewsGetter.hpp"
#include "Util/Log.hpp"

#include "Views/MainMenu/MainMenuView.hpp"

NewsGetter::NewsGetter()
{
    LOGGER.debug("New news getter");
    setHost("netpanzer.org");
    setPort("80");
    setPath("/npnews.txt");
    doRequest();
}

void NewsGetter::onSocketError(network::TCPSocket *so)
{
    HTTPClientSocket::onSocketError(so);
    LOGGER.debug("news getter error");
    MainMenuView::setNews("Error loading news");
}

void NewsGetter::onContentFinished()
{
    MainMenuView::setNews(content);
}
