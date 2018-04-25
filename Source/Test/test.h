
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

#include <string>

namespace Core
{
    struct dunno {};
    namespace Utils {
        template <typename T>
        class sth
        {
            int theField;
        };
    }
}

namespace koza::kwas
{
    enum Meta(Serialize) MyEnum
    {
        None = 0,
        Sth = 1,
        Koza = 2,
        Default = Koza | Sth
    };

    class Serialize
    {
        using test = Core::Utils::sth<Core::dunno>;

        test FunIsFun;

    public:
        std::string Name;
    };

    using AliasSerialize = Serialize;

    typedef Serialize TypedefSerialize;

    class Meta(Serialize) MyGreatClass
    {
        [[nodiscard]]
        auto sth() -> bool;

    public:
         MyGreatClass();
        ~MyGreatClass();

        MyGreatClass& operator==(const MyGreatClass&) = delete;
        MyGreatClass& operator==(MyGreatClass&&) = default;

        float Float;
        int*  pInt;

        Meta(Serialize)
        void ThePublic();

        int normalMethod(float f, bool* b);

        Core::dunno  byValue();
        Core::dunno* byPtr();
        Core::dunno& byRef();

        Core::Utils::sth<Core::dunno> getTemplate();
        void setTemplate(Core::Utils::sth<Core::dunno>& sth);

        template <typename T>
        void theTemplatedMethod(T* something);
    };

    // Surprise!
    int MyGreatClass::normalMethod(float f, bool* b)
    {

    }

    static_assert(true);
}
