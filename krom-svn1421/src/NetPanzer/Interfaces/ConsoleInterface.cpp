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

#include "Interfaces/ConsoleInterface.hpp"

#include "cstring"

#include <time.h>
#include "Interfaces/GameConfig.hpp"
#include "Util/Log.hpp"
#include "Console.hpp"
#include "Resources/ResourceManager.hpp"

bool ConsoleInterface::stdout_pipe;

long ConsoleInterface::console_size;
ConsoleLineArray ConsoleInterface::line_list;

iXY ConsoleInterface::surface_size;
iRect    ConsoleInterface::bounds;
iXY ConsoleInterface::line_offset;
long     ConsoleInterface::vertical_spacing;
long     ConsoleInterface::horizontal_spacing;
long     ConsoleInterface::max_char_per_line;

long ConsoleInterface::line_index;

int ConsoleInterface::commandPos;
void ConsoleInterface::initialize( long size )
{
    assert( size > 0 );

    console_size = size;
    line_list.initialize( size );

    line_index = console_size - 1;

    surface_size = iXY( 800, 600 );
    bounds = iRect( 5, 5, 800 - 5, 600 - 5 );

    max_char_per_line = (bounds.max.x - bounds.min.x) / 8;

    vertical_spacing = 2;

    line_offset.x = 0;
    line_offset.y = (14 + vertical_spacing);

    long line_loop;

    for ( line_loop = 0; line_loop < console_size; line_loop++ )
    {
        line_list[ line_loop ].color = Color::white;
        line_list[ line_loop ].str[0] = 0;
        line_list[ line_loop ].life_timer.changePeriod( 30 );
        line_list[ line_loop ].visible = false;
    }
    commandPos = 0;
    stdout_pipe = false;
}

void ConsoleInterface::setToSurfaceSize( iXY pix )
{
    surface_size = pix;

    bounds.min.x = 5;
    bounds.min.y = 5;
    bounds.max = pix - iXY(5, 5);

    int CHAR_XPIX = 8; // XXX hardcoded
    max_char_per_line = (bounds.max.x - bounds.min.x) / CHAR_XPIX;
}

void ConsoleInterface::setStdoutPipe( bool on_off )
{
    stdout_pipe = on_off;
}

void ConsoleInterface::update( Surface &surface )
{
    update_overlap( surface );
}

void ConsoleInterface::postMessage(PIX msgcolor, bool hasFlag, FlagID flag, const char *format, ...)
{
    char temp_str[256];
    char *temp_str_ptr;
    long temp_str_length;
    va_list vap;

    va_start( vap, format );
    vsnprintf( temp_str, 256, format, vap );
    va_end( vap );

    LOGGER.debug("C: %s", temp_str);

    if (stdout_pipe && Console::server) {
        *Console::server << temp_str << std::endl;
    }

    temp_str_ptr = temp_str;
    temp_str_length = (long) strlen(temp_str);

    if( temp_str_length > max_char_per_line ) {
        long partion_count = temp_str_length / max_char_per_line;

        for( int i = 0; i < partion_count; i++ ) {
            if (line_index == 0)
                line_index = console_size - 1;
            else
                line_index = (line_index - 1) % console_size;

            memset(line_list[ line_index ].str, 0, 256);
            strncpy( line_list[ line_index ].str, temp_str_ptr, max_char_per_line);
            line_list[ line_index ].str[ max_char_per_line ] = 0;

            line_list[ line_index ].color = msgcolor;
            line_list[ line_index ].visible = true;
            line_list[ line_index ].life_timer.reset();
            line_list[ line_index ].hasFlag = hasFlag;
            line_list[ line_index ].flag = flag;

            temp_str_ptr = temp_str_ptr + max_char_per_line;
        }

    }

    if (line_index == 0)
        line_index = console_size - 1;
    else
        line_index = (line_index - 1) % console_size;

    strcpy( line_list[ line_index ].str, temp_str_ptr );

    line_list[ line_index ].color = msgcolor;
    line_list[ line_index ].visible = true;
    line_list[ line_index ].hasFlag = hasFlag;
    line_list[ line_index ].flag = flag;
    line_list[ line_index ].life_timer.reset();
}

void ConsoleInterface::update_overlap( Surface &surface )
{
    iXY current_line;
    long  index;
    short visible_count;

    index = line_index;

    visible_count = 0;

    do {
        if ( line_list[ index ].life_timer.count() ) {
            line_list[ index ].visible = 0;
        }

        if ( line_list[ index ].visible )
            visible_count++;

        index = (index + 1) % console_size;

    } while( index != line_index );

    current_line.y = bounds.min.y + (line_offset.y * visible_count );
    current_line.x = bounds.min.x + line_offset.x;

    int flagextrax;
    Surface * flag = 0;
    do
    {
        if ( line_list[ index ].visible )
        {
            if ( line_list[ index ].hasFlag )
            {
                flag = ResourceManager::getFlag(line_list[index].flag);
                flagextrax = flag->getWidth()+2;
                flag->blt(surface, current_line.x, current_line.y);
            }
            else
            {
                flagextrax = 0;
            }
            // XXX some values by hand
            surface.bltStringShadowed(current_line.x + flagextrax, current_line.y+4, line_list[ index ].str, line_list[ index ].color, Color::black );

            current_line.y = current_line.y - line_offset.y;
        }

        index = (index + 1) % console_size;

    } while( index != line_index );

}
