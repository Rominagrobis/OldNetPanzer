/*
Copyright (C) 2011 by Aaron Perez <aaronps@gmail.com>

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

 * Created on September 1, 2011, 05:01 PM
 */

#ifndef INPUTFIELD_HPP
#define INPUTFIELD_HPP

#include "Component.hpp"
#include "Util/NTimer.hpp"
#include <string>

class InputField : public Component
{
protected:
    virtual void draw(Surface &dest);
    virtual void actionPerformed(const mMouseEvent &me);

    void drawCursor(Surface& dest, int text_y);
    
    void render();

public:
    InputField();
    virtual ~InputField() {}

    void setText(const std::string& text)
    {
        this->text = text;
        fixCursorPos();
    }

    const std::string& getText() const { return text; }

    void setSize(int x, int y)
    {
        (void)y;
        Component::setSize(x+4, y);
    }

    void setTextColor(PIX color) { text_color = color; }
    
    void setMaxTextLength(int max_length) { max_chars = max_length; }
    void setExcludedChars(const std::string& excluded_chars) { this->excluded_chars = excluded_chars; }
    
    virtual void handleKeyboard();

    void handleNormalKey(int key);
    void handleSpecialKey(int key);

private:
    bool has_focus;
    int cursor_pos;
    int max_chars;
    int text_display_start;
    std::string text;
    std::string excluded_chars;

    int last_pressed_key;
    bool was_special_key;
    NTimer key_repeat_timer;
    
    PIX text_color;

    void checkRepeatKey();
    void fixCursorPos();

};

#endif // INPUTFIELD_HPP
