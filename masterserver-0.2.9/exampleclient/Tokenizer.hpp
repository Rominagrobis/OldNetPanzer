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
    Tokenizer(const std::string& newbuffer)
        : buffer(newbuffer), pos(0)
    {
    }

    std::string getNextToken()
    {
        if(pos >= buffer.length())
            return "";

        // skip \ chars
        while(buffer[pos] == '\\' && pos < buffer.length())
            ++pos;
        std::string::size_type start = pos;
        // search next \ char
        while(buffer[pos] != '\\' && pos < buffer.length())
            ++pos;
        
        return std::string(buffer, start, pos-start);
    }

private:
    const std::string& buffer;
    std::string::size_type pos;
};

#endif

