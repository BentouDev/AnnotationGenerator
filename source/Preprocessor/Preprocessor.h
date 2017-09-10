//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "Options.h"

class Preprocessor
{
public:
    Preprocessor(const Options &options);

    int run();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
