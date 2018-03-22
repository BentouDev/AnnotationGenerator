//
// Created by bentoo on 10.09.17.
//

#include "Utils/ArgumentParser.h"
#include "Core/Preprocessor.h"

int main(int argc, char *argv[])
{
    Context        context;
    ArgumentParser arg_parser(context);

    if (!arg_parser.parse(argc, argv))
        return 0;

    Preprocessor preprocessor(context);
    return preprocessor.Run();
}
