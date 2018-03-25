# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Preprocessor VERSION 0.1 LANGUAGES CXX C)

add_executable(Preprocessor main.cpp Utils/ArgumentParser.cpp Utils/ArgumentParser.h Utils/Argument.cpp Utils/Argument.h Core/Preprocessor.cpp Core/Preprocessor.h Core/Context.cpp Core/Context.h Utils/Utils.h Core/SourceFile.cpp Core/SourceFile.h Utils/Filesystem.h Core/SourcePattern.cpp Core/SourcePattern.h)
set_target_properties(Preprocessor PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17)

target_link_libraries(Preprocessor AnnotationLibrary ${LIBCLANG_LIBRARIES} stdc++fs)