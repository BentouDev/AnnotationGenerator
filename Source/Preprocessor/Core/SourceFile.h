//
// Created by bentoo on 24.03.18.
//

#ifndef ANNOTATIONGENERATOR_SOURCEFILE_H
#define ANNOTATIONGENERATOR_SOURCEFILE_H

#include "Utils/Filesystem.h"

class SourceFile
{
public:
    SourceFile(const std::string& path);

    fs::path Path;
};


#endif //ANNOTATIONGENERATOR_SOURCEFILE_H
