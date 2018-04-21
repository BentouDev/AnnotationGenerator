# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Preprocessor VERSION 0.1 LANGUAGES CXX C)

add_executable(Preprocessor main.cpp Utils/ArgumentParser.cpp Utils/ArgumentParser.h Utils/Argument.cpp Utils/Argument.h Core/Parser.cpp Core/Parser.h Core/Context.cpp Core/Context.h Utils/Utils.h Core/SourceFile.cpp Core/SourceFile.h Utils/Filesystem.h Core/SourcePattern.cpp Core/SourcePattern.h Core/Runtime.cpp Core/Runtime.h Core/Generator.cpp Core/Generator.h Core/Visitor.cpp Core/Visitor.h)
set_target_properties(Preprocessor PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17)

target_compile_definitions(Preprocessor PRIVATE -DANNOTATION_GENERATOR)

target_include_directories(Preprocessor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Reflection/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> )

target_link_libraries(Preprocessor Reflection ${LIBCLANG_LIBRARIES} stdc++fs)