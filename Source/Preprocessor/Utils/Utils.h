//
// Created by bentoo on 13.09.17.
//

#ifndef ANNOTATIONGENERATOR_UTILS_H
#define ANNOTATIONGENERATOR_UTILS_H

#include <algorithm>
#include <cstring>
#include <string>
#include <stack>
#include <cmath>

#define countof( X ) \
    std::extent<decltype(X)>::value

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __COUNTER__)

#define DISPOSE auto MAKE_UNIQUE(scope_exit_) [[maybe_unused]] = [&]()
#define UNUSED(x) (void)(x)

namespace Utils
{
    template <typename T>
    bool MatchesAny(const std::string& text, const T& collection)
    {
        return std::any_of(std::begin(collection), std::end(collection), [&](const auto& first){
            return text == first;
        });
    }

    inline std::string Trim(std::string &str)
    {
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        return str;
    }

    template<typename T, typename Container = std::deque<T>>
    class IterableStack
            : public std::stack<T, Container>
    {
        using std::stack<T, Container>::c;

    public:

        // expose just the iterators of the underlying container
        auto begin() { return std::begin(c); }
        auto end() { return std::end(c); }

        auto begin() const { return std::begin(c); }
        auto end() const { return std::end(c); }
    };

    inline unsigned GetNumberOfDigits (unsigned i)
    {
        return i > 0 ? (int) log10 ((double) i) + 1 : 1;
    }

    struct _Color
    {
        int color;
        int bold;
    };

    inline std::ostream& operator<<(std::ostream& os, const _Color& col)
    {
        if (col.color == -1)
            os << "\033[0m";
        else 
            os << "\033[" << col.bold << ";" << col.color << "m";

        return os;
    }

    inline _Color setColor(int color, int bold)
    {
        return _Color{ color, bold };
    }

    inline _Color resetColor()
    {
        return _Color{ -1, -1 };
    }
}

#endif //ANNOTATIONGENERATOR_UTILS_H
