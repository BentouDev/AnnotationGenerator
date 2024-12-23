//
// Created by bentoo on 10.09.17.
//

#include "Utils/ArgumentParser.h"
#include "Core/Runtime.h"

int main(int argc, char** argv)
{
    Data::Context  context;
    ArgumentParser arg_parser(context);


    if (!arg_parser.Parse(argc, const_cast<const char**>(argv)))
        return -1;

    Runtime runtime(context);
    return runtime.Run();
}
