//
// Created by bentoo on 14.04.18.
//

#ifndef ANNOTATIONGENERATOR_REFLECTIONUTILS_H
#define ANNOTATIONGENERATOR_REFLECTIONUTILS_H

#ifndef ANNOTATION_GENERATOR
template <typename T>
using TReflectionVector = const std::vector<T>;
#else
template <typename T>
using TReflectionVector = std::vector<std::shared_ptr<T>>;
#endif

#endif //ANNOTATIONGENERATOR_REFLECTIONUTILS_H
