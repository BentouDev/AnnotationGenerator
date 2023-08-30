//
// Created by bentoo on 27.03.18.
//

#include "Runtime.h"
#include "Generator.h"
#include "Context.h"
#include "Parsing/CursorHandlerFactory.h"
#include "Parsing/Parser.h"
#include "Utils/Utils.h"

#include <memory>
#include <iomanip>
#include <cmath>

Runtime::Runtime(Data::Context& context)
: Context(context)
{
    Context.Parser.GlobalFactory = std::make_unique<CursorHandlerFactory>();
    Context.Parser.TypeFactory   = std::make_unique<CursorHandlerFactory>();
    Context.Parser.AnnotFactory  = std::make_unique<CursorHandlerFactory>();

    Context.Parser.GlobalFactory->RegisterHandlers({
        { CXCursor_EnumDecl,      Handlers::HandleEnum },
        { CXCursor_ClassDecl,     Handlers::HandleType },
        { CXCursor_StructDecl,    Handlers::HandleType },
        { CXCursor_ClassTemplate, Handlers::HandleType },
        { CXCursor_TypedefDecl,   Handlers::HandleTypeAlias },
        { CXCursor_TypeAliasDecl, Handlers::HandleTypeAlias },
        { CXCursor_Namespace,     Handlers::HandleNamespace },
        { CXCursor_VarDecl,       Handlers::HandleVarDecl }
    });

    Context.Parser.TypeFactory->RegisterHandlers({
        { CXCursor_EnumConstantDecl, Handlers::HandleEnumValue },
        { CXCursor_EnumDecl,         Handlers::HandleEnum },
        { CXCursor_ClassDecl,        Handlers::HandleType },
        { CXCursor_StructDecl,       Handlers::HandleType },
        { CXCursor_ClassTemplate,    Handlers::HandleType },
        { CXCursor_TypedefDecl,      Handlers::HandleTypeAlias },
        { CXCursor_TypeAliasDecl,    Handlers::HandleTypeAlias },
        { CXCursor_FieldDecl,        Handlers::HandleField },
        { CXCursor_FunctionDecl,     Handlers::HandleMethod },
        { CXCursor_CXXMethod,        Handlers::HandleMethod },
        { CXCursor_AnnotateAttr,     Handlers::HandleAnnotation },
    });

    Context.Parser.AnnotFactory->RegisterHandlers({
        { CXCursor_CallExpr,      Handlers::SkipRecurse },
        { CXCursor_UnexposedExpr, Handlers::SkipRecurse },
        { CXCursor_StringLiteral, Handlers::HandlePreAnnot },
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

    auto count = static_cast<unsigned>(pattern.Sources.size());
    int  index = 1;
    for (auto& file : pattern.Sources)
    {
        float perc = 100.f * float(index) / float(count);
        std::cout
        << "[" << std::setw(3) << std::fixed << std::setprecision(0) << perc << "%] "
        << Utils::setColor(36, 0)
        << "Processing file " << file->Path
        << Utils::resetColor()
        << std::endl;

        ParseSourceFile(pattern, *file);

        index++;
    }

    std::cout
        << "[100%] "
        << Utils::setColor(36, 1)
        << "Generating source..."
        << Utils::resetColor()
        << std::endl;

    Generator generator(Context);
    generator.GenerateFiles();
}

void Runtime::ParseSourceFile(SourcePattern& pattern, SourceFile& file)
{
    Context.Parser.CurrentSource = &file;
    Context.Parser.CurrentPattern = &pattern;
    Context.Parser.IncludeSourceHeader = pattern.IncludeSourceHeader;

    Parser parser(Context);
    parser.ProcessFile();
}
