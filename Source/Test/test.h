
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)));

#include <string>

namespace koza::kwas
{
    class Serialize
    {
        using test = Core::Utils::sth<Core::dunno>;

        test FunIsFun;

    public:
        std::string Name;
    }

    Meta(Serialize)
    class MyGreatClass
    {
        [[nodiscard]]
        auto sth() -> bool;

    public:
         MyGreatClass();
        ~MyGreatClass();

        operator==(const MyGreatClass&) = delete;
        operator==(MyGreatClass&&) = default;

        float Float;
        int* pInt;
    };

    static_assert(true);
}
