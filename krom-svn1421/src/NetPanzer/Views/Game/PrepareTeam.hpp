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

#ifndef PREPARETEAM_HPP
#define PREPARETEAM_HPP

#include <vector>
#include "GameTemplateView.hpp"
#include "2D/Surface.hpp"
#include "Views/Components/tVScrollBar.hpp"
#include "Views/Components/tPlayerStateBox.hpp"

class Button;

class PrepareTeam : public GameTemplateView
{
private:
    bool loaded;

    iRect firstrect;
    iRect secondrect;
    iRect rect;
    void drawTeams(Surface &dest);
    void DrawInfo(Surface &dest);
    iXY menuImageXY;
    Surface menuImage, vsImage;
    Button * changebutton;
    Button * readybutton;
    tVScrollBar * scTeam1;
    tPlayerStateBox * StateTeam1;
    tVScrollBar * scTeam2;
    tPlayerStateBox * StateTeam2;
public:
    PrepareTeam();
    virtual ~PrepareTeam()
    {}

    virtual void doActivate();
    virtual void doDeactivate();
    virtual void doDraw(Surface &windowArea, Surface &clientArea);
    virtual void checkResolution(iXY oldResolution, iXY newResolution);
    virtual void processEvents();
    void init();

    void changeTeam();
    void playerReady();
    void resetReady();
};

#endif // PREPARETEAM_HPP
