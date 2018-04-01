//
// Created by bentoo on 10.09.17.
//

#include "Parser.h"
#include "../Utils/Utils.h"
#include "Context.h"
#include <clang-c/Index.h>
#include <mustache.hpp>
#include <cstring>
#include <fstream>
#include <clang-c/CXString.h>

Parser::Parser(Data::Context& context)
: Context(context)
{ }

std::vector<cstring> Parser::BuildArguments()
{
    std::vector<cstring> result =
    {
        "-std=c++17",
        "-nostdinc",
        "-nostdinc++",
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
    return "#include \"" + filepath.string() + "\"";
}

CXUnsavedFile Parser::BuildWorkerFile(const std::string& content)
{
    CXUnsavedFile worker_file{};
    worker_file.Filename = "__temp.cpp";
    worker_file.Contents = content.c_str();
    worker_file.Length   = content.size();

    return worker_file;
}

namespace runtime
{
    std::string getCursorKindName(CXCursorKind cursorKind)
    {
        CXString kindName  = clang_getCursorKindSpelling(cursorKind);
        std::string result = clang_getCString(kindName);

        clang_disposeString(kindName);
        return result;
    }

    std::string getCursorSpelling(CXCursor cursor)
    {
        CXString cursorSpelling = clang_getCursorSpelling(cursor);
        std::string result      = clang_getCString(cursorSpelling);

        clang_disposeString(cursorSpelling);
        return result;
    }

    fs::path GetCursorSourcePath(CXCursor cursor)
    {
        CXSourceLocation location = clang_getCursorLocation(cursor);
        CXFile           source{};
        fs::path         source_path;

        clang_getExpansionLocation(location, &source, nullptr, nullptr, nullptr);

        source_path += reinterpret_cast<const char*>(clang_getFileName(source).data);

        return source_path;
    }

    CXChildVisitResult visitor_routine(CXCursor cursor, CXCursor /* parent */, CXClientData data)
    {
        auto* context = reinterpret_cast<Data::Context*>(data);
        auto  result  = CXChildVisit_Continue;
        auto  source  = GetCursorSourcePath(cursor);

        if (source.compare(context->Parser.CurrentSource->Path) == 0)
        {
            return result;
        }

        CXCursorKind cursorKind = clang_getCursorKind(cursor);

        unsigned int curLevel = context->Parser.CurrentLevel;

        std::cout << std::string(curLevel, '-') << " "
                  << getCursorKindName(cursorKind)
                  << " (" << getCursorSpelling(cursor)
                  << ")" << std::endl;

//        switch (cursorKind)
//        {
//            case CXCursor_MacroDefinition:
//                break;
//
//            case CXCursor_EnumConstantDecl:
//                //typeTrav->AddEnumCursor( cursor );
//                break;
//
//            case CXCursor_StructDecl:
//            case CXCursor_ClassDecl:
//                //typeTrav->AddNewTypeCursor( cursor );
//                result = CXChildVisit_Recurse;
//                break;
//
//            case CXCursor_TypedefDecl:
//            case CXCursor_Namespace:
//                result = CXChildVisit_Recurse;
//                break;
//        }

        clang_visitChildren(cursor, visitor_routine, data);

        context->Parser.CurrentLevel++;

        return result;
    }
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

    if (!worker_unit)
    {
        std::cerr << "Unable to create translation unit from file "
                  << Context.Parser.CurrentSource->Path << "."
                  << std::endl;
        return;
    }

    CXCursor root = clang_getTranslationUnitCursor(worker_unit);

    clang_visitChildren(root, runtime::visitor_routine, &Context);

    clang_disposeTranslationUnit(worker_unit);
    clang_disposeIndex(main_index);
}
//    CXTranslationUnit    worker_unit = clang_createTranslationUnitFromSourceFile
//    (
//        main_index,
//        Context.Parser.CurrentSource->Path.c_str(),
//        (int) arguments.size(),
//        arguments.data(),
//        0, nullptr
//    );
