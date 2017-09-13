# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(AnnotationLibrary VERSION 0.1 LANGUAGES CXX C)

add_library(AnnotationLibrary include/ReflectionInfo.h)
set_target_properties(AnnotationLibrary PROPERTIES LINKER_LANGUAGE CXX)