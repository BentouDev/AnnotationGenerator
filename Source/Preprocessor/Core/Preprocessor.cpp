//
// Created by bentoo on 10.09.17.
//

#include "Preprocessor.h"
#include "../Utils/Utils.h"
#include <clang-c/Index.h>
#include <mustache.hpp>
#include <cstring>
#include <fstream>

Preprocessor::Preprocessor(const Context& options)
: CurrentContext(options), CurrentPattern(nullptr)
{ }

int Preprocessor::Run()
{
    for (auto& pattern : CurrentContext.Templates)
    {
        ProcessPattern(*pattern);
    }

    return 0;
}

void Preprocessor::ProcessPattern(SourcePattern& pattern)
{
    CurrentPattern = &pattern;

    for (auto& file : pattern.Sources)
    {
        ProcessFile(*file);
    }
}

std::vector<cstring> Preprocessor::BuildArguments(const std::string& path_arg)
{
    std::vector<cstring> result =
    {
        "-std=c++11",
        "-Wmicrosoft",
        "-Wunknown-pragmas",
        "-D_DEBUG=1",
        path_arg.c_str()
    };

    return result;
}

fs::path Preprocessor::BuildOutputPath(const fs::path& filepath)
{
    return "gen_" + filepath.stem().string() + ".cpp";
}

std::string Preprocessor::BuildFileContents(const fs::path& filepath)
{
    return "#include \"" + filepath.string() + "\"";
}

void Preprocessor::ProcessFile(SourceFile& file)
{
    std::string          path_arg    = "-I" + file.Path.string();
    std::vector<cstring> arguments   = BuildArguments(path_arg);
    fs::path             outputPath  = BuildOutputPath(file.Path);
    std::string          fileContent = BuildFileContents(file.Path);

    CXUnsavedFile generatedFile{};
    generatedFile.Filename = outputPath.c_str();
    generatedFile.Contents = fileContent.c_str();
    generatedFile.Length   = fileContent.size();

    CXIndex CIdx = clang_createIndex(1, 1);
    CXTranslationUnit translationUnit = clang_parseTranslationUnit
    (
        CIdx, outputPath.c_str(),
        arguments.data(),
        (int) arguments.size(),
        &generatedFile,
        1, CXTranslationUnit_None
    );

    // output this new file into the same directory, so user can just #include "gen_header.h"
    // Or... do we actually need this?
    // No!
    // We can output to cpp and generate func that will load whole reflection at once!
    // And then just #include "reflection.h" and Meta::LoadReflectionData();
    // We should call getOutputPath(path) so we can have a different folder for generated cpp's
}
