//
// Created by bentoo on 10.09.17.
//

#include "ArgumentParser.h"
#include "Options.h"
#include <cstdint>

ArgumentParser::ArgumentParser(Options& options)
: Context(options)
{ }

bool ArgumentParser::parse(int argc, char **argv)
{
    for (std::uint32_t i = 0; i < argc; i++)
    {
        Context.FilePaths.emplace_back(argv[i]);
    }

    return true; // ToDo
}
