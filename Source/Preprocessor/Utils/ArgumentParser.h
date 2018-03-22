//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_ARGUMENTPARSER_H
#define ANNOTATIONGENERATOR_ARGUMENTPARSER_H

#include <vector>
#include "Argument.h"
#include "Context.h"

class ArgumentParser
{
    Context& CurrentContext;

public:
    bool parse(int argc, char* argv[]);

    explicit ArgumentParser(Context &options);
};

#endif //ANNOTATIONGENERATOR_ARGUMENTPARSER_H
