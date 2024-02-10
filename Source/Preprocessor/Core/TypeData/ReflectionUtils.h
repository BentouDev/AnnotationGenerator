//
// Created by bentoo on 14.04.18.
//

#ifndef ANNOTATIONGENERATOR_REFLECTIONUTILS_H
#define ANNOTATIONGENERATOR_REFLECTIONUTILS_H

#include <vector>
#include <memory>
#include <cstdint>

#ifndef ANNOTATION_GENERATOR

template <typename T>
using TReflectionVector = std::vector<T>;

template <typename T>
using TRef = const T*;

class CompileString
{
    const char* data;

public:
    template <std::size_t size>
    constexpr CompileString(const char (*str)[size])
    {
        data = (*str);
    }

    const char* c_str()
    {
        return data;
    }
};

#else
template <typename T>
using TReflectionVector = std::vector<std::shared_ptr<T>>;
template <typename T>
using TRef = std::shared_ptr<T>;
using CompileString = std::string;
#endif

#endif //ANNOTATIONGENERATOR_REFLECTIONUTILS_H
