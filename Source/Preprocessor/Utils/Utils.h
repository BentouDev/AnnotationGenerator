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
    bool MatchesAny(std::string_view text, const T& collection)
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

    inline void SplitBySpace(std::string_view input, std::vector<std::string_view>& out)
    {
        bool inside_quotes = false;
        unsigned section_start = 0;

        for (unsigned i = 0; i < unsigned(input.size()); i++)
        {
            char c = input[i];
            if (c == ' ' && !inside_quotes)
            {
                out.push_back(std::string_view(input.data() + section_start, i - section_start));
                section_start = i + 1;
            }
            else if (c == '"')
            {
                inside_quotes = !inside_quotes;
            }
        }
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
        return i > 0U ? (unsigned int) log10 ((double) i) + 1U : 1U;
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
