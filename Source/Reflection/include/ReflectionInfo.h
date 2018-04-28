//
// Created by bentoo on 11.09.17.
//
////////////////////////////////////////////////////////////////////////////////////
#ifndef ANNOTATIONGENERATOR_REFLECTIONINFO_H
#define ANNOTATIONGENERATOR_REFLECTIONINFO_H
////////////////////////////////////////////////////////////////////////////////////

#include <string>
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

class MetaInfo
{
public:
    virtual ~MetaInfo() = default;

    std::string Name;
};

////////////////////////////////////////////////////////////////////////////////////

using TypeId = int;

class TypeInfo : public MetaInfo
{
public:
    virtual ~TypeInfo() = default;

    TypeId Id;
};

////////////////////////////////////////////////////////////////////////////////////

class FieldInfo : public MetaInfo
{
public:
    TRef<TypeInfo>    BaseType;
    AccessType::TYPE  AccessType;
    ValueType::TYPE   ValueType;

    template <typename T>
    T* GetValue();

    template <typename T>
    void SetValue(T* value);
};

////////////////////////////////////////////////////////////////////////////////////

class MethodInfo : public MetaInfo
{
public:
    TRef<TypeInfo>              ReturnType;
    TReflectionVector<TypeInfo> Parameters;
};

////////////////////////////////////////////////////////////////////////////////////

class ClassInfo : public TypeInfo
{
public:
    ClassInfo*  Super;
    std::string CanonName;

    REF_CONST std::vector<std::string> Annotations;
    TReflectionVector<ClassInfo>       Interfaces;
    TReflectionVector<FieldInfo>       Fields;
    TReflectionVector<MethodInfo>      Methods;
};

////////////////////////////////////////////////////////////////////////////////////

class EnumValueInfo : public MetaInfo
{
public:
    template <typename T>
    T* GetValue();
};

class EnumInfo : public TypeInfo
{
public:
    TRef<TypeInfo>                   Super;
    TReflectionVector<EnumValueInfo> Values;
};

////////////////////////////////////////////////////////////////////////////////////
#endif //ANNOTATIONGENERATOR_REFLECTIONINFO_H
////////////////////////////////////////////////////////////////////////////////////