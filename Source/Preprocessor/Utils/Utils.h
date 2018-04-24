//
// Created by bentoo on 13.09.17.
//

#ifndef ANNOTATIONGENERATOR_UTILS_H
#define ANNOTATIONGENERATOR_UTILS_H

#include <algorithm>
#include <cstring>
#include <string>
#include <stack>

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
        return std::any_of(std::begin(collection), std::end(collection), [&](const char* first){
            return std::strcmp(first, text.c_str()) == 0;
        });
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
}

#endif //ANNOTATIONGENERATOR_UTILS_H
