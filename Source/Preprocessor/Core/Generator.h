//
// Created by bentoo on 27.03.18.
//

#ifndef ANNOTATIONGENERATOR_GENERATOR_H
#define ANNOTATIONGENERATOR_GENERATOR_H


#include "Context.h"

class Generator
{
    Data::Context& Context;

public:
    explicit Generator(Data::Context& context);

    void GenerateFiles();
};


#endif //ANNOTATIONGENERATOR_GENERATOR_H
