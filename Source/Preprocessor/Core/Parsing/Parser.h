//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "Core/Context.h"
#include "Utils/Filesystem.h"

#include <clang-c/Index.h>

class SourceFile;

class Parser
{
    Data::Context& Context;

    auto BuildArguments() -> std::vector<cstring>;
    auto BuildWorkerFileContent(const fs::path& filepath) -> std::string;
    auto BuildWorkerFile(const std::string& content) -> CXUnsavedFile;

    std::vector<std::unique_ptr<std::string>> Arguments;

public:
    explicit Parser(Data::Context& context);
    void ProcessFile();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
