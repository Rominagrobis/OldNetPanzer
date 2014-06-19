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

#include <vector>
#include "2D/Surface.hpp"
#include "GameTemplateView.hpp"

class PlayerState;

class RankTeam : public GameTemplateView
{
private:
    void drawTeamStats(Surface &dest);
    std::vector<const PlayerState*> states;
    int selected_line;
    int selected_col;
    iRect RectStates;
public:
    RankTeam();
    virtual void doDraw(Surface &windowArea, Surface &clientArea);
protected:
    virtual void doActivate();
    virtual void doDeactivate();
    virtual void mouseMove(const iXY & prevPos, const iXY &newPos);
    virtual void notifyMoveTo();
    virtual void processEvents();
};
