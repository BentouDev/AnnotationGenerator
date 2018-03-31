//
// Created by bentoo on 27.03.18.
//

#include "Runtime.h"
#include "Parser.h"
#include "Context.h"

Runtime::Runtime(Data::Context& context)
: Context(context)
{

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
