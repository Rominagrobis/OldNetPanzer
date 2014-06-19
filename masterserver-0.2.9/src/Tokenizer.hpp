/*
Copyright (C) 2004 Matthias Braun <matze@braunis.de>
 
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
#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include <iostream>

/** a class that splits a string into multiple tokens that are separated by a
 * \ delimiter character
 */
class Tokenizer
{
public:
    Tokenizer(std::istream& newin)
        : in(newin)
    {
    }

    std::string getNextToken()
    {
        if(in.eof()) {
            return "";
        }
        
        std::string result;

        char c;
        do {
            in.get(c);
        } while(!in.eof() && c == '\\');

        if(in.eof())
            return "";
        
        do {
            result += c;
            in.get(c);
        } while((!in.eof()) && c != '\\');

        return result;
    }

private:
    std::istream& in;
};

#endif

