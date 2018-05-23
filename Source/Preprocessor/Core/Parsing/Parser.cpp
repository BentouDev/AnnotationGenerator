//
// Created by bentoo on 10.09.17.
//

#include "Parser.h"
#include "../Context.h"
#include "Visitor.h"
#include "../../Utils/Utils.h"

#include <clang-c/Index.h>
#include <iostream>
#include <fstream>

Parser::Parser(Data::Context& context)
: Context(context)
{ }

std::vector<cstring> Parser::BuildArguments()
{
    std::vector<cstring> result =
    {
        "-std=c++17",
        "-Wmicrosoft",
        "-Wunknown-pragmas",
        "-D_DEBUG=1"
    };

    for (auto& include : Context.Parser.Includes)
    {
        result.push_back(include.c_str());
    }

    return result;
}

std::string Parser::BuildWorkerFileContent(const fs::path& filepath)
{
    // todo: optimize memory usage
    std::stringstream ss;
    std::ifstream     file(filepath);
    std::string       line;

    while (std::getline(file, line))
    {
        if (line.find_first_of("#include") == std::string::npos)
            ss << line;
    }

    return ss.str();

    // return "#include \"" + filepath.string() + "\"";
}

CXUnsavedFile Parser::BuildWorkerFile(const std::string& content)
{
    CXUnsavedFile worker_file{};
    worker_file.Filename = "__temp.cpp";
    worker_file.Contents = content.c_str();
    worker_file.Length   = (unsigned long) content.size();

    return worker_file;
}

void Parser::ProcessFile()
{
    if (!fs::exists(Context.Parser.CurrentSource->Path))
    {
        std::cerr << "Unable to open source file "
                  << Context.Parser.CurrentSource->Path << "."
                  << std::endl;
        return;
    }

    fs::path             directory   = Context.Parser.CurrentSource->Path.parent_path();
    std::string          path_arg    = "-I" + directory.string();
    std::vector<cstring> arguments   = BuildArguments();
    std::string          fileContent = BuildWorkerFileContent(Context.Parser.CurrentSource->Path);
    CXUnsavedFile        worker_file = BuildWorkerFile(fileContent);
    CXIndex              main_index  = clang_createIndex(0, 1);
    CXTranslationUnit    worker_unit = clang_parseTranslationUnit
    (
        main_index,
        worker_file.Filename,
        arguments.data(),
        (int) arguments.size(),
        &worker_file,
        1, CXTranslationUnit_None
    );

    DISPOSE
    {
        clang_disposeTranslationUnit(worker_unit);
        clang_disposeIndex(main_index);
    };

    if (!worker_unit)
    {
        std::cerr << "Unable to create translation unit from file "
                  << Context.Parser.CurrentSource->Path << "."
                  << std::endl;
        return;
    }

    CXCursor root = clang_getTranslationUnitCursor(worker_unit);

    Visitor visitor(Context);
    visitor.VisitChildren(root);
}
