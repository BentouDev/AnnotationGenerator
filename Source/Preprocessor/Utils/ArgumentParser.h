//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_ARGUMENTPARSER_H
#define ANNOTATIONGENERATOR_ARGUMENTPARSER_H

#include <vector>
#include "Argument.h"
#include "Options.h"

class ArgumentParser
{
    std::vector<Argument> Arguments;

    Options& Context;

public:
    bool parse(int argc, char* argv[]);

    ArgumentParser(Options &options);
};

#endif //ANNOTATIONGENERATOR_ARGUMENTPARSER_H
