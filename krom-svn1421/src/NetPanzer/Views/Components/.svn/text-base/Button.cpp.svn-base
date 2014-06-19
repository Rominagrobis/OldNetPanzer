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


#include "Views/Components/Button.hpp"
#include "Views/Components/View.hpp"
#include "Views/Theme.hpp"
#include "Util/Log.hpp"

#include "MouseEvent.hpp"
#include "Actions/Action.hpp"

Button::Button()
{
    setTextColors(Color::white, Color::red, Color::yellow, Color::darkGray);
    position.zero();
    label.clear();
    bstate = BNORMAL;
    memset(borders, 0, sizeof(borders));
    extraBorder = 0;
    clickAction = 0;
    dirty = true;
}

Button::~Button()
{
    if ( clickAction && ! clickAction->isShared() )
    {
        delete clickAction;
        clickAction = 0;
    }
}

void
Button::setAction(Action* action)
{
    if ( clickAction && ! clickAction->isShared() )
    {
        delete clickAction;
    }

    clickAction = action;
}

// render
void
Button::render()
{
    surface.fill(0);
    dirty = false;
    if (!visible) return;
    if ( bimage.getNumFrames() == 1 )
    {
        bimage.bltTrans(surface, extraBorder + state_offset[bstate].x, extraBorder + state_offset[bstate].y);
    }
    else if ( bimage.getNumFrames() >= 3 )
    {
        bimage.setFrame( (bstate < bimage.getNumFrames()) ? bstate : 0 );
        bimage.bltTrans(surface, extraBorder + state_offset[bstate].x, extraBorder + state_offset[bstate].y);
    }
    else
    {
        surface.fill(componentBodyColor);
    }

    if ( borders[bstate][0]|extraBorder ) // only 1 | (binary or)
    {
        surface.drawButtonBorder(borders[bstate][0], borders[bstate][1]);
    }

    if ( label.length() )
    {
        Surface text;
        text.renderText( label.c_str(), textColors[bstate], 0);
        // blit centered and transparent
        text.bltTrans(surface, ((surface.getWidth()/2) - (text.getWidth()/2))+state_offset[bstate].x,
                      ((surface.getHeight()/2) - (text.getHeight()/2))+state_offset[bstate].y);
    }

}

// actionPerformed
//---------------------------------------------------------------------------
void
Button::actionPerformed(const mMouseEvent &me)
{
    if ( bstate == BDISABLED || !visible)
    {
        return;
    }

    if (me.getID() == mMouseEvent::MOUSE_EVENT_ENTERED
                || me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED)
    {
        bstate = BOVER;
        dirty = true; // draw text in red
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_EXITED)
    {
        bstate = BNORMAL;
        dirty = true; // draw defaults;
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
    {
        bstate = BPRESSED;
        dirty = true;
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_CLICKED)
    {
        if ( clickAction )
        {
            clickAction->execute();
        }
    }
} // end Button::actionPerformed

Button *
Button::createTextButton( const NPString& label,
                            const iXY& loc,
                            const int bwidth,
                            Action * action)
{
    Button * b = new Button();
    b->setLabel(label);
    b->setLocation(loc);
    b->setTextButtonSize(bwidth);
    b->setNormalBorder();
    b->setAction(action);
    b->setTextColors(componentInActiveTextColor, componentFocusTextColor, componentActiveTextColor, componentInActiveTextColor);
    b->setStateOffset(Button::BPRESSED, 1, 1);

    if ( ! action )
    {
        LOGGER.warning("No action defined for button '%s'", label.c_str());
    }
    return b;
}

Button *
Button::createNewSpecialButton(    const NPString& label,
                                   const iXY& loc,
                                   int width,
                                   Action * action)
{
    Surface bitmap;
    bitmap.loadBMP(itButton);

    Surface bstart;
    bstart.grab(bitmap, iRect(0, 0, 15, bitmap.getHeight()));
    Surface bend;
    bend.grab(bitmap, iRect(bitmap.getWidth()-15, 0, bitmap.getWidth(), bitmap.getHeight()));
    Surface bmiddle;
    bmiddle.grab(bitmap, iRect(15, 0, bitmap.getWidth()-15, bitmap.getHeight()));

    Surface spbutton(width, bstart.getHeight(), 1);

    spbutton.setFrame(0);
    bstart.blt(spbutton,0,0);
    int msize = bmiddle.getWidth();
    for (int i = 0; i < (int)((spbutton.getWidth())/msize);i++)
    {
        bmiddle.blt(spbutton,15+(msize*i),0);
    }
    bend.blt(spbutton,spbutton.getWidth()-15,0);

    Button *b = new Button();
    b->setImage(spbutton);
    b->setLabel(label);
    b->setLocation(loc);
    b->setTextColors(ctTexteNormal, ctTexteOver, ctTextePressed, ctTexteDisable);
    b->setStateOffset(Button::BPRESSED, 1, 1);

    b->setAction(action);

    return b;
}
