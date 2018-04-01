//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include <clang-c/Index.h>
#include "Context.h"
#include "../Utils/Filesystem.h"

class SourceFile;

class Parser
{
    Data::Context& Context;

    auto BuildArguments() -> std::vector<cstring>;
    auto BuildWorkerFileContent(const fs::path& filepath) -> std::string;
    auto BuildWorkerFile(const std::string& content) -> CXUnsavedFile;

public:
    explicit Parser(Data::Context& context);
    void ProcessFile();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
