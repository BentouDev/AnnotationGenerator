//
// Created by bentoo on 24.03.18.
//

#ifndef ANNOTATIONGENERATOR_FILESYSTEM_H
#define ANNOTATIONGENERATOR_FILESYSTEM_H

#if __cpp_lib_filesystem || __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __cpp_lib_experimental_filesystem || __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error "AnnotationGenerator requires filesystem TS which seems to be lacking!"
#endif

using cstring = const char*;


#endif //ANNOTATIONGENERATOR_FILESYSTEM_H
