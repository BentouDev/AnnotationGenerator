#include <string>

class [[ rtti::class, first, second(x[y[0]]()), foo([](){}), third(Dupa) ]] Dupa
{

};

enum class [[rtti::enum]] MyEnum
{
    Once,
    Twice,
    Trice
};

namespace EOldEnum
{
    enum [[rtti::enum]] TYPE
    {
        None,
        Default = 0,
        NoMore = 1,
    };
};

$YClass()
class MyData
{
    std::string name;
    int value;
};

class [[gnu::always_inline]] [[gnu::hot]] [[gnu::const]] [[nodiscard]] First
{

};

class [[gnu::always_inline, gnu::const, gnu::hot, nodiscard]] Second
{

};

class
 [[using gnu : const, always_inline, hot]] [[nodiscard]] 
 Third
{

};

class [[rtti::class]] WithData
{
    float f;
    std::string s;
    int i;
};
