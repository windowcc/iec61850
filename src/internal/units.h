#ifndef __UNITS_H__
#define __UNITS_H__

#include <string>

void stringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace(pos, srclen, strdst );
        pos += dstlen;
    }
}



#endif // ! __UNITS_H__