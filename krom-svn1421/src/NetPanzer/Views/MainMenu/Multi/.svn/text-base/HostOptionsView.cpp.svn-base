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

#include "HostOptionsView.hpp"
#include "Interfaces/StrManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Particles/ParticleSystemGlobals.hpp"
#include "MapSelectionView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Views/Components/Label.hpp"
#include "Views/Components/Button.hpp"
#include "Views/Components/Desktop.hpp"
#include "Actions/Action.hpp"
#include "Actions/ChangeIntVarAction.hpp"

int HostOptionsView::cloudCoverageCount = 1;
int HostOptionsView::windSpeed          = 1;
int HostOptionsView::gameType           = 0;

std::string HostOptionsView::cloudCoverageString;
std::string HostOptionsView::windSpeedString;

static int getCurMaxPlayersCount()
{
    return GameConfig::game_maxplayers;
}

static int  getCurMaxUnitCount()
{
    return GameConfig::game_maxunits;
}

void HostOptionsView::updateGameConfigCloudCoverage()
{
    switch (cloudCoverageCount)
    {
        case 1:
            cloudCoverageString = _("Broken");
            GameConfig::game_cloudcoverage = cloudCoverageCount;
            break;
        case 2:
            cloudCoverageString = _("Partly Cloudy");
            GameConfig::game_cloudcoverage = cloudCoverageCount;
            break;
        case 3:
            cloudCoverageString = _("Overcast");
            GameConfig::game_cloudcoverage = cloudCoverageCount;
            break;
        case 4:
            cloudCoverageString = _("Extremely Cloudy");
            GameConfig::game_cloudcoverage = cloudCoverageCount;
            break;
        default:
            cloudCoverageString = _("Clear");
            GameConfig::game_cloudcoverage = 0;
            break;
    }
}

void HostOptionsView::updateGameConfigGameType()
{
    switch (gameType) {
    case 0: {
            GameConfig::game_gametype = _gametype_objective;
        }
        break;

    case 1: {
            GameConfig::game_gametype = _gametype_fraglimit;
        }
        break;

    case 2: {
            GameConfig::game_gametype = _gametype_timelimit;
        }
        break;

    }

}

static const char * getGameTypeString()
{
    switch ( GameConfig::game_gametype ) {
    case _gametype_objective: {
            return( _("Objective") );
        }
        break;

    case _gametype_fraglimit : {
            return( _("Time Limit") );
        }
        break;

    case _gametype_timelimit : {
            return( _("Frag Limit") );
        }
        break;

    }
    return( _("Unknown") );
}



static int getTimeLimitHours()
{
    return GameConfig::game_timelimit / 60;
}

static int getTimeLimitMinutes()
{
    return GameConfig::game_timelimit % 60;
}



static int getFragLimit()
{
    return GameConfig::game_fraglimit;
}

void HostOptionsView::updateWindSpeedString()
{
    float calmWindSpeed    = float(baseWindSpeed) * calmWindsPercentOfBase;
    float breezyWindSpeed  = float(baseWindSpeed) * breezyWindsPercentOfBase;
    float briskWindSpeed   = float(baseWindSpeed) * briskWindsPercentOfBase;
    float heavyWindSpeed   = float(baseWindSpeed) * heavyWindsPercentOfBase;
    float typhoonWindSpeed = float(baseWindSpeed) * typhoonWindsPercentOfBase;

    switch (windSpeed) {
    case 0: {
            windSpeedString = _("Calm");
            GameConfig::game_windspeed = int(calmWindSpeed);
        }
        break;
    case 1: {
            windSpeedString = _("Breezy");
            GameConfig::game_windspeed = int(breezyWindSpeed);
        }
        break;
    case 2: {
            windSpeedString = _("Brisk Winds");
            GameConfig::game_windspeed = int(briskWindSpeed);
        }
        break;
    case 3: {
            windSpeedString = _("Heavy Winds");
            GameConfig::game_windspeed = int(heavyWindSpeed);
        }
        break;
    case 4: {
            windSpeedString = _("Typhoon");
            GameConfig::game_windspeed = int(typhoonWindSpeed);
        }
        break;
    }
}

static int getObjectiveCapturePercent()
{
    return GameConfig::game_occupationpercentage;
}




// HostOptionsView
//---------------------------------------------------------------------------
HostOptionsView::HostOptionsView() : RMouseHackView()
{
    setSearchName("HostOptionsView");
    setTitle(_("Host Options"));
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    moveTo(bodyTextRect.min.x, bodyTextRect.min.y + 205);
    resizeClientArea(bodyTextRect.getSizeX()-5, 168);

    addMeterButtons(iXY(BORDER_SPACE, BORDER_SPACE));

} // end HostOptionsView::HostOptionsView

// doDraw
//---------------------------------------------------------------------------
void HostOptionsView::doDraw(Surface &viewArea, Surface &clientArea)
{
    drawMeterInfo(clientArea, iXY(BORDER_SPACE, BORDER_SPACE));

    clientArea.bltString( 4, clientArea.getHeight() - Surface::getFontHeight(),
                    _("Note: Use the right mouse button to accomplish fast mouse clicking."),
                    windowTextColor);

    View::doDraw(viewArea, clientArea);

} // end HostOptionsView::doDraw

void HostOptionsView::doDeactivate()
{
    // nothing
}

#define arrowButtonWidth (16)
#define xControlStart (270)

void HostOptionsView::addConfRow(   const iXY pos,
                                    const NPString& label,
                                    Action* decreaseAction,
                                    Action* increaseAction )
{
    iXY p(pos);
    add( new Label(p.x, p.y, label, windowTextColor, windowTextColorShadow, true) );
    p.x += xControlStart - 1;
    add( Button::createTextButton( "<", p, arrowButtonWidth-2, decreaseAction));
    p.x += arrowButtonWidth + meterWidth + 3; // 3 = 1 space + 2 of the border
    add( Button::createTextButton( ">", p, arrowButtonWidth-2, increaseAction));
}

// addMeterButtons
//---------------------------------------------------------------------------
void HostOptionsView::addMeterButtons(const iXY &pos)
{
    const int yOffset          = 15;

    int y = pos.y;
    
    iXY p(pos);
    
    addConfRow(p, _("Max Players"),
                   new ChangeIntVarAction<GameConfig::game_maxplayers, 2, 16>(-1),
                   new ChangeIntVarAction<GameConfig::game_maxplayers, 2, 16>(1) );
//    addConfRow(p, "Max Players", new ChangePlayerCountAction(-1), new ChangePlayerCountAction(1) );
    
    p.y += yOffset;
    y += yOffset;

    addConfRow(p, _("Game Max Unit Count"),
                   new ChangeIntVarAction<GameConfig::game_maxunits, 2, 1000>(-5),
                   new ChangeIntVarAction<GameConfig::game_maxunits, 2, 1000>(5) );

    p.y += yOffset;
    y += yOffset;
    
    addConfRow(p, _("Objective Capture Percent"),
                   new ChangeIntVarAction<GameConfig::game_occupationpercentage, 5, 100>(-5),
                   new ChangeIntVarAction<GameConfig::game_occupationpercentage, 5, 100>(5) );
    
    p.y += yOffset;
    y += yOffset;

    addConfRow(p, _("Time Limit"),
                   new ChangeIntVarAction<GameConfig::game_timelimit, 5, 240>(-5),
                   new ChangeIntVarAction<GameConfig::game_timelimit, 5, 240>(5) );
    p.y += yOffset;
    y += yOffset;
    
    addConfRow(p, _("Frag Limit"),
                   new ChangeIntVarAction<GameConfig::game_fraglimit, 5, 1000>(-5),
                   new ChangeIntVarAction<GameConfig::game_fraglimit, 5, 1000>(5) );
    
    p.y += yOffset;
    y += yOffset;
    
    const int minWidth = 150;
    int xChoiceOffset = 2;

    choiceGameType.setLabel(_("Game Type"));
    choiceGameType.addItem(_("Objective"));
    choiceGameType.addItem(_("Frag Limit"));
    choiceGameType.addItem(_("Time Limit"));
    choiceGameType.setMinWidth(minWidth);
    choiceGameType.setLocation(xChoiceOffset, 100);
    choiceGameType.select( getGameTypeString() );
    add(&choiceGameType);
    xChoiceOffset += minWidth + 123;

    choiceCloudCoverage.setLabel(_("Cloud Coverage"));
    choiceCloudCoverage.addItem(_("Clear"));
    choiceCloudCoverage.addItem(_("Broken"));
    choiceCloudCoverage.addItem(_("Partly Cloudy"));
    choiceCloudCoverage.addItem(_("Overcast"));
    choiceCloudCoverage.addItem(_("Extremely Cloudy"));
    choiceCloudCoverage.setMinWidth(minWidth);
    choiceCloudCoverage.setLocation(xChoiceOffset, 100);
    choiceCloudCoverage.select(cloudCoverageCount);
    add(&choiceCloudCoverage);
    xChoiceOffset += minWidth + 13;

    choiceWindSpeed.setLabel(_("Wind Speed"));
    choiceWindSpeed.addItem(_("Calm"));
    choiceWindSpeed.addItem(_("Breezy"));
    choiceWindSpeed.addItem(_("Brisk Winds"));
    choiceWindSpeed.addItem(_("Heavy Winds"));
    choiceWindSpeed.addItem(_("Typhoon"));
    choiceWindSpeed.setMinWidth(minWidth);
    choiceWindSpeed.setLocation(xChoiceOffset, 100);
    choiceWindSpeed.select(windSpeed);
    add(&choiceWindSpeed);
    xChoiceOffset += minWidth + 10;

    checkPublic.setLabel(_("Public"));
    checkPublic.setState(GameConfig::server_public);
    checkPublic.setLocation(2, 125);
    add(&checkPublic);

    checkPowerUp.setLabel(_("PowerUps"));
    checkPowerUp.setState(GameConfig::game_powerups);
    checkPowerUp.setLocation(120, 125);
    add(&checkPowerUp);
    
} // end HostOptionsView::addMeterButtons

// drawMeterInfo
//---------------------------------------------------------------------------
void HostOptionsView::drawMeterInfo(Surface &dest, const iXY &pos)
{
    char strBuf[256];

//    const int arrowButtonWidth = 16;
    const int yOffset          = 15;

    int x = pos.x + 270 + arrowButtonWidth;
    int y = pos.y;

    Surface tempSurface(meterWidth, 14, 1);
    tempSurface.fill(meterColor);

    // Game Max Player Count
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d", getCurMaxPlayersCount());
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    // Game Max Unit Count
    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d - %d %s", getCurMaxUnitCount(), 
            getCurMaxUnitCount() / getCurMaxPlayersCount(), 
            _("max per player"));
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);
    
    // Objective Capture Percent
    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    MapInfo* m = ((MapSelectionView*)Desktop::getView("MapSelectionView"))->getCurrentSelectedMapInfo();
    if ( m )
    {
        int objectiveCount = m->objectiveCount;
        sprintf(strBuf, "%d%% - %d of %d", getObjectiveCapturePercent(),
                int(float(objectiveCount) * (float(getObjectiveCapturePercent()) / 100.0f) + 0.999),
                objectiveCount);
    } else {
        sprintf(strBuf,"%s", _("Map Data Needed"));
    }
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d:%d", getTimeLimitHours(), getTimeLimitMinutes() );
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);

    y += yOffset;
    tempSurface.fill(meterColor);
    tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    sprintf(strBuf, "%d %s", getFragLimit(), _("Frags") );
    tempSurface.bltStringCenter(strBuf, meterTextColor);
    tempSurface.blt(dest, x, y);


    /*
    	// Fog of War
    	y += yOffset;
    	tempSurface.fill(meterColor);
    	tempSurface.drawButtonBorder(meterTopLeftBorderColor, meterBottomRightBorderColor);
    	sprintf(strBuf, "%s", getAllowFogOfWar());
    	tempSurface.bltStringCenter(strBuf, meterTextColor);
    	tempSurface.blt(dest, x, y);
    */

} // end HostOptionsView::drawMeterInfo

// actionPerformed
//---------------------------------------------------------------------------
void HostOptionsView::actionPerformed(mMouseEvent me)
{
    if (me.getSource()==&checkPublic) {
        if ( getVisible() ) {
            GameConfig::server_public = checkPublic.getState();
        }
    } else if (me.getSource()==&checkPowerUp) {
        if ( getVisible() ) {
            GameConfig::game_powerups = checkPowerUp.getState();
        }
    } else if (me.getSource()==&choiceWindSpeed) {
        windSpeed = choiceWindSpeed.getSelectedIndex();

        updateWindSpeedString();
    } else if (me.getSource()==&choiceCloudCoverage) {
        cloudCoverageCount = choiceCloudCoverage.getSelectedIndex();

        updateGameConfigCloudCoverage();
    } else if (me.getSource()==&choiceGameType) {
        if ( getVisible() ) {
            gameType = choiceGameType.getSelectedIndex();

            updateGameConfigGameType();
        }
    }
} // end HostOptionsView::actionPerformed
