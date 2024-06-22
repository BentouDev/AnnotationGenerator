//
// Created by bentoo on 11.09.17.
//
////////////////////////////////////////////////////////////////////////////////////
#ifndef ANNOTATIONGENERATOR_REFLECTIONINFO_H
#define ANNOTATIONGENERATOR_REFLECTIONINFO_H
////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <any>
#include <vector>
#include "ReflectionUtils.h"

////////////////////////////////////////////////////////////////////////////////////

namespace ValueType
{
    enum TYPE
    {
        Value,
        Pointer,
        Reference
    };
}

namespace AccessType
{
    enum TYPE
    {
        None = 0,
        Readable = 1,
        Writable = 2,
        Mutable = Readable | Writable,
        Const = Readable
    };
}

////////////////////////////////////////////////////////////////////////////////////
class ClassInfo;
namespace Meta
{
    template <typename>
    ClassInfo* GetType()
    { return nullptr; }
}

////////////////////////////////////////////////////////////////////////////////////

class MetaInfo
{
public:
    MetaInfo(CompileString name)
        : Name(name.c_str())
    { }

    virtual ~MetaInfo() = default;

    std::string Name;
};

////////////////////////////////////////////////////////////////////////////////////

using TypeId = int;

class TypeInfo : public MetaInfo
{
public:
    TypeInfo(CompileString name)
        : MetaInfo(name), HasAnyAnnotation(false)
    { }

    std::string CanonName;
    std::string FromInclude;

    std::vector<std::string> Annotations;
    bool HasAnyAnnotation;

    bool HasAnnotation(const std::vector<std::string>& names) const;
    bool HasAnnotation(std::string_view name) const;

    TypeId Id;
};

////////////////////////////////////////////////////////////////////////////////////

class FieldInfo : public MetaInfo
{
public:
    FieldInfo(CompileString name, TRef<TypeInfo> baseType)
        : MetaInfo(name), BaseType(baseType)
    { }

    TRef<TypeInfo>    BaseType;
    AccessType::TYPE  AccessType;
    ValueType::TYPE   ValueType;

    // virtual std::any GetValue(void*) = 0;

    // virtual void SetValue(void*, std::any) = 0;
};

namespace detail
{
    template <typename FieldPtr, FieldPtr ptr>
    class FieldWrapper : public FieldInfo
    {
    public:
        FieldWrapper(CompileString name, TRef<TypeInfo> baseType)
            : FieldInfo(name, baseType)
        { }
    };

    template <typename T, typename F, F T::* Member>
    struct FieldWrapper<F T::*, Member> : public FieldInfo
    {
    public:
        FieldWrapper(CompileString name)
            : FieldInfo(name, Meta::GetType<T>())
        {

        }

//        virtual std::any GetValue(void* inst) override
//        {
//
//        }
    };
}

////////////////////////////////////////////////////////////////////////////////////

class MethodInfo : public MetaInfo
{
public:
    MethodInfo(CompileString name, TRef<TypeInfo> type)
        : MetaInfo(name), ReturnType(type)
    { }

    TRef<TypeInfo>              ReturnType;
    TReflectionVector<TypeInfo> Parameters;
};

namespace detail
{
    template<class MemFunPtrType, MemFunPtrType Method>
    class MethodWrapper : public MethodInfo
    {
    public:
        MethodWrapper(CompileString name, TRef<TypeInfo> type)
            : MethodInfo(name, type)
        { }
    };

    template <class Clazz,
            class ReturnType,
            class...Args,
            ReturnType(Clazz::*Method)(Args...)>

    class MethodWrapper<ReturnType(Clazz::*)(Args...), Method> : public MethodInfo
    {
    public:
        MethodWrapper(CompileString name)
            : MethodInfo(name, Meta::GetType<ReturnType>())
        {
            Parameters = {(Meta::GetType<Args>())...};
        }
    };
}

////////////////////////////////////////////////////////////////////////////////////

class ClassInfo : public TypeInfo
{
public:
    ClassInfo(CompileString name)
        : TypeInfo(name)
    { }

    TReflectionVector<ClassInfo>   BaseClasses;
    TReflectionVector<FieldInfo>   Fields;
    TReflectionVector<MethodInfo>  Methods;
};

////////////////////////////////////////////////////////////////////////////////////

class EnumValueInfo : public MetaInfo
{
public:
    EnumValueInfo(CompileString name, long long value)
        : MetaInfo(name), NumValue(value)
    { }

    long long NumValue;

    template <typename T>
    T* GetValue();
};

class EnumInfo : public TypeInfo
{
public:
    EnumInfo(CompileString name)
        : TypeInfo(name)
    { }

    TRef<TypeInfo>                   Super;
    TReflectionVector<EnumValueInfo> Values;
};

////////////////////////////////////////////////////////////////////////////////////
#endif //ANNOTATIONGENERATOR_REFLECTIONINFO_H
////////////////////////////////////////////////////////////////////////////////////