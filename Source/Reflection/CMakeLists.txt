# Clang based C++ preprocessor for annotation based reflection generation 
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Reflection VERSION 0.1 LANGUAGES CXX C)

add_library(Reflection STATIC src/ReflectionInfo.cpp include/ReflectionInfo.h include/ReflectionUtils.h)
set_target_properties(Reflection PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 11)

install(TARGETS Reflection
        ARCHIVE  DESTINATION lib
        LIBRARY  DESTINATION lib
        RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY include/ DESTINATION include)

target_include_directories(Reflection PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> 
        PRIVATE src)

set_target_properties(Reflection PROPERTIES LINKER_LANGUAGE CXX)
export(TARGETS Reflection FILE ReflectionConfig.cmake)