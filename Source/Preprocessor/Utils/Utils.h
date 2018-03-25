//
// Created by bentoo on 13.09.17.
//

#ifndef ANNOTATIONGENERATOR_UTILS_H
#define ANNOTATIONGENERATOR_UTILS_H

#include <algorithm>
#include <cstring>

#define countof( X ) \
    std::extent<decltype(X)>::value

namespace Utils
{
    template <typename T>
    bool MatchesAny(const std::string& text, T collection)
    {
        return std::any_of(std::begin(collection), std::end(collection), [&](const char* first){
            return std::strcmp(first, text.c_str()) == 0;
        });
    }
}

#endif //ANNOTATIONGENERATOR_UTILS_H
