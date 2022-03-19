#include "string_util.h"

std::string takeUntil(std::string const &s, char ch) {
    std::string::size_type pos = s.find(ch);
    if (pos != std::string::npos)
        return s.substr(0, pos);
    else
        return s;
}
