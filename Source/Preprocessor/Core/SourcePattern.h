//
// Created by bentoo on 25.03.18.
//

#ifndef ANNOTATIONGENERATOR_CODETEMPLATE_H
#define ANNOTATIONGENERATOR_CODETEMPLATE_H

#include <memory>
#include <vector>
#include "SourceFile.h"

class SourcePattern
{
public:
    std::vector<std::string> Templates;
    std::vector<std::unique_ptr<SourceFile>> Sources;
};

#endif //ANNOTATIONGENERATOR_CODETEMPLATE_H
