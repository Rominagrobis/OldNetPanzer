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

#include <fstream>

#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Interfaces/StrManager.hpp"

typedef std::map<NPString, NPString> t_tblLanguage;
static t_tblLanguage tblLanguage;
static bool isLngLoaded = false;


std::string UTF8ToUnicode(const std::string text)
{
    std::string UTF8;
    size_t pos = 0;
    while (pos <= text.size())
    {
        int count;
        int utf8;
        char c = text[pos++];


        if (!(c & 0x80))
        {
            UTF8 += c;// ascii
            continue;
        }

        if ((c & 0xE0) == 0xC0)
        {
            utf8 = (c & 0x1F);
            count = 1;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            utf8 = (c & 0x0F);
            count = 2;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            utf8 = (c & 0x07);
            count = 3;
        }
        else if ((c & 0xFC) == 0xF8)
        {
            utf8 = (c & 0x03);
            count = 4;
        }
        else if (( c & 0xFE) == 0xFC)
        {
            utf8 = (c & 0x01);
            count = 5;
        }
        else
        {
            LOGGER.debug("Invalid utf8");
            continue;
        }

        while (count--)
        {
            c = text[pos++];
            if ((c & 0xC0) != 0x80)
            {
                LOGGER.debug("Invalid utf8");
                continue;
            }
            utf8 <<= 6;
            utf8 |= (c & 0x3F);
        }
        UTF8 += utf8-34;
    }
    return UTF8;
}

void loadPOFile(const NPString& fileName)
{
    NPString nplFile = "Languages/"+fileName;
    std::ifstream in(nplFile.c_str());
    enum { EMPTY, MSGID, MULTIMSGID, MSGSTR, MULTIMSGSTR } state;
    NPString msgid, msgstr;
    state = EMPTY;

    if (!in.is_open())
    {
        LOGGER.warning("Language not found: %s\n", nplFile.c_str());
        return;
    }
    LOGGER.warning("Loading language %s\n", nplFile.c_str());

    while(!in.eof())
    {
        NPString line, currmsg;
        getline(in, line);
        const char *c = line.c_str();

        while(isspace(*c) && *c != 0)
        {
            c++;
        };
        if( (*c == 0) || (*c =='#') )
            continue; // skip blank lines or comment

        if (!strncmp(c, "msgid ", 6))
        {
            if ((state == MSGSTR || state == MULTIMSGSTR) && !msgid.empty())
            {
                if (msgstr.empty())
                    tblLanguage[msgid] = msgid;
                else
                    tblLanguage[msgid] = UTF8ToUnicode(msgstr);
            }
            state = MSGID;
            msgid.clear();
        }

        if (!strncmp(c, "msgstr ", 7))
        {
            state = MSGSTR;
            msgstr.clear();
        }

        while(*c != 0 && *c != '"')
        {
            ++c;
        }
        if(*c != '"')
            continue;
        c++;
        while(*c != 0 && *c != '"')
        {
            if (*c == '\\')
            {
                ++c;
                if (*c)
                {
                    if (*c == 'n')
                    {
                        currmsg += '\n';
                    }
                    else if (*c == 't')
                    {
                        currmsg += '\t';
                    }
                    else if (*c == 'r')
                    {
                        currmsg += '\r';
                    }
                    else if (*c == '"')
                    {
                        currmsg += '"';
                    }
                    else if (*c == '\\')
                    {
                        currmsg += '\\';
                    }
                    else
                    {
                        LOGGER.debug("LoadPO : Invalid escape character: %c\n", *c);
                    }
                    ++c;
                }
                else
                {
                    LOGGER.debug("LoadPO : Unterminated string\n");
                }
            }
            else
            {
                currmsg += *c++;
            }
        }

        if (state == MSGID)
        {
            msgid = currmsg;
            state = MULTIMSGID;
        }
        else if (state == MULTIMSGID)
        {
            msgid += currmsg;
        }
        else if (state == MSGSTR)
        {
            msgstr = currmsg;
            state = MULTIMSGSTR;
        }
        else if (state == MULTIMSGSTR)
        {
            msgstr += currmsg;
        }
    }
    isLngLoaded = true;
    in.close();
}

const char* Translate(const NPString& str)
{
    if (!isLngLoaded) return str.c_str();
    t_tblLanguage::iterator i = tblLanguage.find(str);
    if (i != tblLanguage.end() && !i->second.empty())
    {
        return i->second.c_str();
    }
    else
    {
        return str.c_str();
    }
}







