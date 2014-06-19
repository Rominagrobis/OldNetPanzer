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
#include <config.h>

#include "Section.hpp"

#include <ctype.h>
#include <sstream>
#include "Log.hpp"

namespace iniparser
{

using masterserver::log;

Section::Section(const std::string& newname)
    : name(newname)
{
}

Section::~Section()
{
}

const std::string&
Section::getName() const
{
    return name;
}

bool
Section::exists(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator i = values.find(key);

    return i != values.end();
}

const std::string&
Section::getValue(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator i = values.find(key);
    if(i == values.end()) {
        *log << "Couldn't find key '" << key << "' in config.\n";
        static std::string nokey;
        return nokey;
    }

    return i->second;
}

int
Section::getIntValue(const std::string& key) const
{
    const std::string& value = getValue(key);
    std::stringstream str(value);

    int result;
    str >> result;
    return result;
}

float
Section::getFloatValue(const std::string& key) const
{
    const std::string& value = getValue(key);
    std::stringstream str(value);

    float result;
    str >> result;
    return result;
}

bool
Section::getBoolValue(const std::string& key) const
{
    std::string value = getValue(key);
    for(int i = 0; i < value.size(); ++i) {
        value[i] = tolower(value[i]);
    }
    if(value == "yes" || value == "true" || value == "1" || value == "on") {
        return true;
    } else if(value == "no" || value == "false" || value == "0" || value == "off") {
        return false;
    }
    
    std::cout << "Warning couldn't parse value '" << 
        getValue(key) << "' as bool value\n";
    return false;
}

void
Section::setValue(const std::string& key, const std::string& value)
{
    std::map<std::string, std::string>::iterator i = values.find(key);
    if(i == values.end()) {
        values.insert(std::make_pair(key, value));
    } else {
        i->second = value;
    }
}

void
Section::setIntValue(const std::string& key, int value)
{
    std::stringstream str;
    str << value;

    setValue(key, str.str());
}

void
Section::setFloatValue(const std::string& key, float value)
{
    std::stringstream str;
    str << value;

    setValue(key, str.str());
}

void
Section::setBoolValue(const std::string& key, bool value)
{
    setValue(key, value ? "yes" : "no");
}

} // end of namespace INI
