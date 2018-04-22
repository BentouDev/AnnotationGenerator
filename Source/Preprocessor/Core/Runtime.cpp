//
// Created by bentoo on 27.03.18.
//

#include "Runtime.h"
#include <memory>
#include "Parsing/Parser.h"
#include "Parsing/CursorHandlerFactory.h"
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
       { CXCursor_Namespace,     Handlers::HandleNamespace }
    });

    Context.Parser.TypeFactory->RegisterHandlers({
        { CXCursor_EnumDecl,      Handlers::HandleEnum },
        { CXCursor_ClassDecl,     Handlers::HandleType },
        { CXCursor_StructDecl,    Handlers::HandleType },
        { CXCursor_ClassTemplate, Handlers::HandleType },
        { CXCursor_FieldDecl,     Handlers::HandleField },
        { CXCursor_FunctionDecl,  Handlers::HandleMethod }
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
    for (auto& file : pattern.Sources)
    {
        ParseSourceFile(*file);
    }
}

void Runtime::ParseSourceFile(SourceFile& file)
{
    Context.Parser.CurrentSource = &file;

    Parser parser(Context);
    parser.ProcessFile();
}
