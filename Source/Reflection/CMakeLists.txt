# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Reflection VERSION 0.1 LANGUAGES CXX C)

add_library(Reflection include/ReflectionInfo.h)

install(TARGETS Reflection
        ARCHIVE  DESTINATION lib
        LIBRARY  DESTINATION lib
        RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY include/ DESTINATION include)

target_include_directories(Reflection PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> )

set_target_properties(Reflection PROPERTIES LINKER_LANGUAGE CXX)
export(TARGETS Reflection FILE ReflectionConfig.cmake)