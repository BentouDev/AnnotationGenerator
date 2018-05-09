
#include "meta_MyGreatClass.h"


namespace Meta
{
    void RegisterReflection()
    {
        
        Meta::GetType<typename koza::kwas::MyGreatClass>();
        
    }
}