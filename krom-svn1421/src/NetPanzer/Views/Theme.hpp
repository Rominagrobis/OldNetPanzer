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
#ifndef __Theme_hpp__
#define __Theme_hpp__

//Color theme
#define ctWindowsBorder             (15)
#define ctWindowsbackground         (33)

#define ctTexteNormal               (Color::lightGray)
#define ctTexteOver                 (174)
#define ctTextePressed              (174)
#define ctTexteDisable              (15)

// Image theme
#define itButton                    ("pics/backgrounds/menus/buttons/default/b-black.bmp")
#define itScroll                    ("pics/backgrounds/menus/buttons/default/scrollbutton.bmp")

    //TODO: Load theme from ini file
//void LoadNPTheme();

#endif // __Theme_hpp__
