//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "../Utils/Context.h"

class Preprocessor
{
    const Context& CurrentContext;

    void ProcessFile(const std::string& filepath);

public:
    explicit Preprocessor(const Context &options);

    int Run();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
