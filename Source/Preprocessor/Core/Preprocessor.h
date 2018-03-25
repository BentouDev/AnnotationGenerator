//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_PREPROCESSOR_H
#define ANNOTATIONGENERATOR_PREPROCESSOR_H

#include "Context.h"
#include "../Utils/Filesystem.h"

class SourceFile;

class Preprocessor
{
    const Context& CurrentContext;
    SourcePattern* CurrentPattern;

    void ProcessPattern(SourcePattern& pattern);
    void ProcessFile(SourceFile& file);
    auto BuildArguments(const std::string& path_arg) -> std::vector<cstring>;
    auto BuildOutputPath(const fs::path& filepath) -> fs::path;
    auto BuildFileContents(const fs::path& filepath) -> std::string;

public:
    explicit Preprocessor(const Context &options);

    int Run();
};

#endif //ANNOTATIONGENERATOR_PREPROCESSOR_H
