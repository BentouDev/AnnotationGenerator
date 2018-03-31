//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "Context.h"
#include "../Utils/Filesystem.h"

class SourceFile;

class Parser
{
    const Data::Context& Context;

    auto BuildArguments(const std::string& path_arg) -> std::vector<cstring>;
    auto BuildOutputPath(const fs::path& filepath) -> fs::path;
    auto BuildFileContents(const fs::path& filepath) -> std::string;

public:
    explicit Parser(const Data::Context& context);
    void ProcessFile();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
