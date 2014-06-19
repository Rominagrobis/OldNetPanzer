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

#ifndef __Button_hpp__
#define __Button_hpp__

#include "Component.hpp"
#include "2D/Color.hpp"
#include "ViewGlobals.hpp"

class Action;

//--------------------------------------------------------------------------
class Button : public Component
{
public:
    typedef enum
    {
        BNORMAL =   0,
        BOVER   =   1,
        BPRESSED=   2,
        BDISABLED=  3,
        BMAX_STATE
    } ButtonState;
    
private:
    int extraBorder;
    Action* clickAction;
    
protected:
    
    typedef enum
    {
        BORDER_TOP_LEFT    = 0,
        BORDER_BOTTOM_RIGHT = 1,
        BORDER_COLOR_MAX
    } BorderColorDef;
    
    std::string label;
       
    PIX textColors[BMAX_STATE];
    iXY state_offset[BMAX_STATE];
   
    Surface bimage;
   
    void render();
    ButtonState bstate;
   
    void resetState()
    {
        bstate = BNORMAL;
        dirty = true;
    }

public:
    PIX borders[BMAX_STATE][BORDER_COLOR_MAX];
    
    Button();

    virtual ~Button();

    static Button * createTextButton(   const NPString& label,
                                        const iXY& loc,
                                        const int bwidth,
                                        Action * action);

    static Button * createNewSpecialButton(     const NPString& label,
                                                const iXY& loc,
                                                int width,
                                                Action * action);

    void setAction( Action * action );
    
    void setTextColors( PIX normal, PIX over, PIX pressed, PIX disabled)
    {
        textColors[BNORMAL] = normal;
        textColors[BOVER] = over;
        textColors[BPRESSED] = pressed;
        textColors[BDISABLED] = disabled;
    }

    void setLabel(const std::string& l)
    {
        label = l;
        dirty = true;
    }
   
    void setImage(const Surface &s)
    {
        if ( s.getNumFrames() )
        {
            bimage.copy(s);
            setSize(bimage.getWidth(), bimage.getHeight());
        }
        dirty = true;
    }
    
    void enable()
    {
        if ( bstate == BDISABLED )
        {
            bstate = BNORMAL;
            dirty = true;
        }
    }
    
    void disable()
    {
        if ( bstate != BDISABLED )
        {
            bstate = BDISABLED;
            dirty = true;
        }
    }
    
    bool isEnabled()
    {
        return bstate != BDISABLED;
    }
    
    void setUnitSelectionBorder()
    {
        setExtraBorder();
        borders[BNORMAL][BORDER_TOP_LEFT]       = Color::darkGray;
        borders[BNORMAL][BORDER_BOTTOM_RIGHT]   = Color::darkGray;
        borders[BOVER][BORDER_TOP_LEFT]         = Color::red;
        borders[BOVER][BORDER_BOTTOM_RIGHT]     = Color::red;
        borders[BPRESSED][BORDER_TOP_LEFT]      = Color::darkGray;
        borders[BPRESSED][BORDER_BOTTOM_RIGHT]  = Color::darkGray;
        borders[BDISABLED][BORDER_TOP_LEFT]     = Color::darkGray;
        borders[BDISABLED][BORDER_BOTTOM_RIGHT] = Color::darkGray;
        dirty = true;
    }
   
    void setNormalBorder()
    {
        setExtraBorder();
        borders[BNORMAL][BORDER_TOP_LEFT]       = topLeftBorderColor;
        borders[BNORMAL][BORDER_BOTTOM_RIGHT]   = bottomRightBorderColor;
        borders[BOVER][BORDER_TOP_LEFT]         = topLeftBorderColor;
        borders[BOVER][BORDER_BOTTOM_RIGHT]     = bottomRightBorderColor;
        borders[BPRESSED][BORDER_TOP_LEFT]      = bottomRightBorderColor;
        borders[BPRESSED][BORDER_BOTTOM_RIGHT]  = topLeftBorderColor;
        borders[BDISABLED][BORDER_TOP_LEFT]     = Color::darkGray;
        borders[BDISABLED][BORDER_BOTTOM_RIGHT] = Color::darkGray;
        dirty = true;
    }

    void setRedGreenBorder()
    {
        setExtraBorder();
        borders[BNORMAL][BORDER_TOP_LEFT]       = 0;
        borders[BNORMAL][BORDER_BOTTOM_RIGHT]   = 0;
        borders[BOVER][BORDER_TOP_LEFT]         = Color::red;
        borders[BOVER][BORDER_BOTTOM_RIGHT]     = Color::darkRed;
        borders[BPRESSED][BORDER_TOP_LEFT]      = Color::green;
        borders[BPRESSED][BORDER_BOTTOM_RIGHT]  = Color::darkGreen;
        borders[BDISABLED][BORDER_TOP_LEFT]     = Color::darkGray;
        borders[BDISABLED][BORDER_BOTTOM_RIGHT] = Color::darkGray;
        dirty = true;
    }

    void setStateOffset(ButtonState state, int x, int y)
    {
        state_offset[state].x = x;
        state_offset[state].y = y;
    }
    
    void clearBorder()
    {
        memset(borders, 0, sizeof(borders));
        dirty = true;
    }
   
    void setSize(int x, int y)
    {
        Component::setSize(x+(extraBorder*2), y+(extraBorder*2));
    }

    void setTextButtonSize(int xsize)
    {
        Component::setSize(xsize+(extraBorder*2), Surface::getFontHeight() + 4 + (extraBorder*2));
    }
   
    void setExtraBorder()
    {
        if ( !extraBorder )
        {
            extraBorder = 1;
            setSize( size.x, size.y);
        }
    }
   
    void clearExtraBorder()
    {
        if ( extraBorder )
        {
            extraBorder = 0;
            setSize( size.x, size.y);
        }
    }

    const std::string& getLabel() const
    {
        return label;
    }

    virtual void actionPerformed(const mMouseEvent &me);
}; // end Button

#endif // end __Button_hpp__
