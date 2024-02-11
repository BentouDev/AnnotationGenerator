#include "class.h"
#include "class.gen.h"

namespace Meta
{
    void RegisterReflection();
}

int main()
{
    Meta::RegisterReflection();
    return 0;
}
