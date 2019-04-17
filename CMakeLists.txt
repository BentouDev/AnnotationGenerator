# Clang based C++ preprocessor for annotation based reflection generation 
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(AnnotationGenerator VERSION 0.3 LANGUAGES CXX C)

# Add modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules" ${CMAKE_MODULE_PATH})

# Enforce warnings
if(MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3")
else()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif(MSVC)

# Find clang
find_package(LibClang REQUIRED)
include_directories(AnnotationGenerator ${LIBCLANG_INCLUDE_DIRS})

# Download Conan.cmake automatically
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.13/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# Add dependencies
conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP 
                BUILD missing)

add_subdirectory(Source/Reflection)
add_subdirectory(Source/Preprocessor)