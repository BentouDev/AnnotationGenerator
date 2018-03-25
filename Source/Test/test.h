
#   define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

#include <string>

class Serialize
{
public:
    std::string Name;
}

Meta(Serialize)
class MyGreatClass
{
public:
     MyGreatClass();
    ~MyGreatClass();
    
    float Float;
    int* pInt;
};
