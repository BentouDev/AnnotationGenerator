//
// Created by bentoo on 27.03.18.
//

#include "Runtime.h"
#include <memory>
#include <iomanip>

#include "../Utils/Utils.h"
#include "Parsing/CursorHandlerFactory.h"
#include "Parsing/Parser.h"
#include "Generator.h"
#include "Context.h"

Runtime::Runtime(Data::Context& context)
: Context(context)
{
    Context.Parser.GlobalFactory = std::make_unique<CursorHandlerFactory>();
    Context.Parser.TypeFactory   = std::make_unique<CursorHandlerFactory>();

    Context.Parser.GlobalFactory->RegisterHandlers({
        { CXCursor_EnumDecl,      Handlers::HandleEnum },
        { CXCursor_ClassDecl,     Handlers::HandleType },
        { CXCursor_StructDecl,    Handlers::HandleType },
        { CXCursor_ClassTemplate, Handlers::HandleType },
        { CXCursor_TypedefDecl,   Handlers::HandleTypeAlias },
        { CXCursor_TypeAliasDecl, Handlers::HandleTypeAlias },
        { CXCursor_Namespace,     Handlers::HandleNamespace },
    });

    Context.Parser.TypeFactory->RegisterHandlers({
        { CXCursor_EnumDecl,      Handlers::HandleEnum },
        { CXCursor_ClassDecl,     Handlers::HandleType },
        { CXCursor_StructDecl,    Handlers::HandleType },
        { CXCursor_ClassTemplate, Handlers::HandleType },
        { CXCursor_TypedefDecl,   Handlers::HandleTypeAlias },
        { CXCursor_TypeAliasDecl, Handlers::HandleTypeAlias },
        { CXCursor_FieldDecl,     Handlers::HandleField },
        { CXCursor_FunctionDecl,  Handlers::HandleMethod },
        { CXCursor_CXXMethod,     Handlers::HandleMethod },
        { CXCursor_AnnotateAttr,  Handlers::HandleAnnotation },
    });
}

int Runtime::Run()
{
    for (auto& pattern : Context.Templates)
    {
        ProcessPattern(*pattern);
    }

    return 0;
}

void Runtime::ProcessPattern(SourcePattern& pattern)
{
    pattern.LoadTemplates();

    Context.Generator.CurrentPattern = &pattern;

    auto count   = static_cast<unsigned>(pattern.Sources.size());
    int  padding = Utils::GetNumberOfDigits(count);
    int  index   = 1;
    for (auto& file : pattern.Sources)
    {
        std::cout
        << "[" << std::setw(padding) << index << "/" << count
        << "] Processing file " << file->Path
        << std::endl << std::flush;

        ParseSourceFile(*file);

        index++;
    }

    Generator generator(Context);
    generator.GenerateFiles();
}

void Runtime::ParseSourceFile(SourceFile& file)
{
    Context.Parser.CurrentSource = &file;

    Parser parser(Context);
    parser.ProcessFile();
}
