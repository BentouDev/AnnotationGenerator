//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "Utils/Options.h"

class Preprocessor
{
    const Options& CurrentOptions;

public:
    explicit Preprocessor(const Options &options);

    int run();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
