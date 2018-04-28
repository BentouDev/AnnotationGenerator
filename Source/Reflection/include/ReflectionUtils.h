//
// Created by bentoo on 14.04.18.
//

#ifndef ANNOTATIONGENERATOR_REFLECTIONUTILS_H
#define ANNOTATIONGENERATOR_REFLECTIONUTILS_H

#include <vector>
#include <memory>

#ifndef ANNOTATION_GENERATOR
#define REF_CONST const
template <typename T>
using TReflectionVector = const std::vector<T>;

template <typename T>
using TRef = const T;
#else
#define REF_CONST
template <typename T>
using TReflectionVector = std::vector<std::shared_ptr<T>>;
template <typename T>
using TRef = std::shared_ptr<T>;
#endif

#endif //ANNOTATIONGENERATOR_REFLECTIONUTILS_H
