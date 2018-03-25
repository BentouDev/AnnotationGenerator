//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_OPTIONS_H
#define ANNOTATIONGENERATOR_OPTIONS_H

#include <vector>
#include <memory>
#include "SourcePattern.h"

class Context
{
public:
    std::vector<std::unique_ptr<SourcePattern>> Templates;
};

#endif //ANNOTATIONGENERATOR_OPTIONS_H
