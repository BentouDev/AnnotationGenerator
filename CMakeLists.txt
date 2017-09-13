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

add_subdirectory(Source/Reflection)
add_subdirectory(Source/Preprocessor)