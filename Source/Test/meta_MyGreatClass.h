#include <Reflection/ReflectionInfo.h>

namespace Meta::ReflectionData
{
    class meta_MyGreatClass : public ClassInfo
    {
        static meta_MyGreatClass Data;

        // private constructor
        meta_MyGreatClass();

        const std::string Name = "MyGreatClass";
        const std::string CanonicalName = "koza::kwas::MyGreatClass";

    public:
        ClassInfo& Get() { return Data; }
    };

    TReflectionVector<FieldInfo> MyGreatClassMeta::Fields = {
        
            { Float, &koza::kwas::MyGreatClass::Float, , Meta::GetType<float> },
        
            { pInt, &koza::kwas::MyGreatClass::pInt, , Meta::GetType<int *> },
        
    };

    TReflectionVector<MethodInfo> MyGreatClassMeta::Methods = {
        
            { sth, &koza::kwas::MyGreatClass::sth, Meta::GetType<bool> },
        
            { operator==, &koza::kwas::MyGreatClass::operator==, Meta::GetType<koza::kwas::MyGreatClass &> },
        
            { operator==, &koza::kwas::MyGreatClass::operator==, Meta::GetType<koza::kwas::MyGreatClass &> },
        
            { ThePublic, &koza::kwas::MyGreatClass::ThePublic, Meta::GetType<void> },
        
            { normalMethod, &koza::kwas::MyGreatClass::normalMethod, Meta::GetType<int> },
        
            { byValue, &koza::kwas::MyGreatClass::byValue, Meta::GetType<dunno> },
        
            { byPtr, &koza::kwas::MyGreatClass::byPtr, Meta::GetType<Core::dunno *> },
        
            { byRef, &koza::kwas::MyGreatClass::byRef, Meta::GetType<Core::dunno &> },
        
            { getTemplate, &koza::kwas::MyGreatClass::getTemplate, Meta::GetType<test> },
        
            { setTemplate, &koza::kwas::MyGreatClass::setTemplate, Meta::GetType<void> },
        
    };
}

template <> ClassInfo* Meta::GetType<typename koza::kwas::MyGreatClass>()
{
    return &Meta::ReflectionData::meta_MyGreatClass::Get();
}