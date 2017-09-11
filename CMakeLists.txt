# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(AnnotationGenerator VERSION 0.1 LANGUAGES CXX C)

# Add modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

# Enforce warnings
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif(CMAKE_COMPILER_IS_GNUCC)
if(MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif(MSVC)

# Find clang
find_package(LibClang REQUIRED)
include_directories(AnnotationGenerator ${LIBCLANG_INCLUDE_DIRS})

add_library(AnnotationLibrary include/ReflectionInfo.h)
set_target_properties(AnnotationLibrary PROPERTIES LINKER_LANGUAGE CXX)

add_executable(AnnotationGenerator source/main.cpp source/Utils/ArgumentParser.cpp source/Utils/ArgumentParser.h source/Utils/Argument.cpp source/Utils/Argument.h source/Preprocessor/Preprocessor.cpp source/Preprocessor/Preprocessor.h source/Preprocessor/Options.cpp source/Preprocessor/Options.h)
set_target_properties(AnnotationGenerator PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(AnnotationGenerator AnnotationLibrary ${LIBCLANG_LIBRARIES})