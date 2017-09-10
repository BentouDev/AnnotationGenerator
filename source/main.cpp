//
// Created by bentoo on 10.09.17.
//

#include "Utils/ArgumentParser.h"
#include "Preprocessor/Preprocessor.h"
#include "Preprocessor/Options.h"

int main(int argc, char *argv[])
{
    Options        options;
    ArgumentParser arg_parser(options);

    if (!arg_parser.parse(argc, argv))
        return 0;

    Preprocessor preprocessor(options);
    return preprocessor.run();
}
