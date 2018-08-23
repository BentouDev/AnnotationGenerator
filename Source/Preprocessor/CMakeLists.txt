# Clang based C++ preprocessor for annotation based reflection generation 
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Preprocessor VERSION 0.1 LANGUAGES CXX C)

add_executable(Preprocessor main.cpp Utils/ArgumentParser.cpp Utils/ArgumentParser.h Utils/Argument.cpp Utils/Argument.h Core/Parsing/Parser.cpp Core/Parsing/Parser.h Core/Context.cpp Core/Context.h Utils/Utils.h Core/SourceFile.cpp Core/SourceFile.h Utils/Filesystem.h Utils/Filesystem.cpp Core/SourcePattern.cpp Core/SourcePattern.h Core/Runtime.cpp Core/Runtime.h Core/Generator.cpp Core/Generator.h Core/Parsing/Visitor.cpp Core/Parsing/Visitor.h Core/Parsing/CursorHandlerFactory.cpp Core/Parsing/CursorHandlerFactory.h Core/Parsing/ParseContext.cpp Core/Parsing/ParseContext.h Core/Parsing/Typedefs.h)
set_target_properties(Preprocessor PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
        OUTPUT_NAME "Agnes")

target_compile_definitions(Preprocessor PRIVATE -DANNOTATION_GENERATOR)

target_include_directories(Preprocessor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Reflection/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> )

if (WIN32)
        set(OS_LIBRARIES )
else()
        set(OS_LIBRARIES stdc++fs)
endif()

target_link_libraries(Preprocessor Reflection ${LIBCLANG_LIBRARIES} ${OS_LIBRARIES})

install(TARGETS Preprocessor
        ARCHIVE  DESTINATION lib
        LIBRARY  DESTINATION lib
        RUNTIME  DESTINATION bin)
