# Clang based C++ preprocessor adding annotation based code generation
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Preprocessor VERSION 0.1 LANGUAGES CXX C)

add_executable(Preprocessor main.cpp Utils/ArgumentParser.cpp Utils/ArgumentParser.h Utils/Argument.cpp Utils/Argument.h Preprocessor.cpp Preprocessor.h Utils/Options.cpp Utils/Options.h)
set_target_properties(Preprocessor PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Preprocessor AnnotationLibrary ${LIBCLANG_LIBRARIES})