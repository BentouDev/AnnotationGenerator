#include "class.h"
#include "class.gen.h"

#include <iostream>>

int main()
{
    std::cout << meta::type<MyClass>::value.name << std::endl;
    return 0;
}
