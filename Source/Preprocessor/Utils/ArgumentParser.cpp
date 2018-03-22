//
// Created by bentoo on 10.09.17.
//

#include "ArgumentParser.h"

ArgumentParser::ArgumentParser(Context& context)
: CurrentContext(context)
{ }

bool ArgumentParser::parse(int argc, char **argv)
{
    // todo: implement proper argument handling
    for (auto i = 1; i < argc; i++)
    {
        CurrentContext.FilePaths.emplace_back(argv[i]);
    }

    return true;
}
