//
// Created by bentoo on 10.09.17.
//

#include "Parser.h"
#include "Visitor.h"
#include "Core/Context.h"
#include "Utils/Utils.h"

#include <clang-c/Index.h>
#include <string_view>
#include <iostream>
#include <fstream>

Parser::Parser(Data::Context& context)
: Context(context)
{ }

std::vector<cstring> Parser::BuildArguments()
{
    std::vector<cstring> result =
    {
        "-std=c++23",
        "-Wmicrosoft",
        "-Wno-unknown-pragmas",
        "-Wno-unknown-attributes",
        "-D_DEBUG=1",
        "-DANNOTATION_GENERATOR"
    };

    for (auto& path : Context.Parser.CurrentPattern->IncludeDirectories)
    {
        Arguments.emplace_back(std::make_unique<std::string>(std::string("-I") + path));
        result.push_back(Arguments.back()->c_str());
    }

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
    
    for (auto& incl : Context.Parser.CurrentPattern->InjectIncludes)
    {
        ss << "#include <" << incl << ">" << std::endl;
    }

    if (Context.Parser.IncludeSourceHeader)
    {
        ss << "#include \"" + filepath.string() + "\"\n";
    }
    else
    {
        std::ifstream     file(filepath);
        std::string       line;

        while (std::getline(file, line))
        {
            auto incl_pos = line.rfind("#include");
            if (incl_pos == std::string::npos)
                ss << line << std::endl;
        }
    }

    return ss.str();
}

CXUnsavedFile Parser::BuildWorkerFile(const std::string& content)
{
    CXUnsavedFile worker_file{};
    worker_file.Filename = Context.Parser.CurrentUnitName.c_str();
    worker_file.Contents = content.c_str();
    worker_file.Length   = (unsigned long) content.size();

    return worker_file;
}

static std::string_view ClangErrorToString(CXErrorCode err)
{
    switch (err)
    {
        case (CXErrorCode::CXError_Success):
        {
            return "Success";
        }
        case (CXErrorCode::CXError_ASTReadError):
        {
            return "ASTReadError";
        }
        case (CXErrorCode::CXError_InvalidArguments):
        {
            return "InvalidArguments";
        }
        case (CXErrorCode::CXError_Failure):
        {
            return "Failure";
        }
        case (CXErrorCode::CXError_Crashed):
        {
            return "Crashed";
        }
    }

    return "Unknown";
}

void Parser::ProcessFile()
{
    if (!fs::exists(Context.Parser.CurrentSource->Path))
    {
        std::cout << "Unable to open source file '"
                  << Context.Parser.CurrentSource->Path << "'."
                  << std::endl;
        return;
    }

    Context.Parser.CurrentUnitName = "__temp.hpp";

    int excludeDeclarationsFromPCH = (int)false;
    int displayDiagnostics         = (int)true;

    fs::path             directory   = Context.Parser.CurrentSource->Path.parent_path();
    std::string          path_arg    = "-I" + directory.string();
    std::vector<cstring> arguments   = BuildArguments();
    std::string          fileContent = BuildWorkerFileContent(Context.Parser.CurrentSource->Path);
    CXUnsavedFile        worker_file = BuildWorkerFile(fileContent);
    CXIndex              main_index  = clang_createIndex(excludeDeclarationsFromPCH, displayDiagnostics);
    CXTranslationUnit    worker_unit = nullptr;
    CXErrorCode err = clang_parseTranslationUnit2
    (
        main_index,
        worker_file.Filename,
        arguments.data(),
        (int) arguments.size(),
        &worker_file,
        1, CXTranslationUnit_IgnoreNonErrorsFromIncludedFiles | CXTranslationUnit_SkipFunctionBodies, //CXTranslationUnit_Incomplete | CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_KeepGoing,
        &worker_unit
    );

    CXFile file_handle = clang_getFile(worker_unit, worker_file.Filename);

    DISPOSE
    {
        clang_disposeTranslationUnit(worker_unit);
        clang_disposeIndex(main_index);
    };

    if (!worker_unit)
    {
        std::string_view reason = ClangErrorToString(err);

        std::cerr << "Unable to create translation unit from file "
                  << Context.Parser.CurrentSource->Path << ". Reason: "
                  << reason
                  << std::endl;
        return;
    }

    Context.Parser.TranslationUnit = &worker_unit;
    Context.Parser.FileHandle = file_handle;

    CXCursor root = clang_getTranslationUnitCursor(worker_unit);

    Visitor visitor(Context);
    visitor.VisitChildren(root);
}
