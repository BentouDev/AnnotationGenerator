//
// Created by bentoo on 10.09.17.
//

#include "Parser.h"
#include "../Utils/Utils.h"
#include <clang-c/Index.h>
#include <mustache.hpp>
#include <cstring>
#include <fstream>

Parser::Parser(const Data::Context& context)
: Context(context)
{ }

std::vector<cstring> Parser::BuildArguments(const std::string& path_arg)
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

fs::path Parser::BuildOutputPath(const fs::path& filepath)
{
    return "gen_" + filepath.stem().string() + ".cpp";
}

std::string Parser::BuildFileContents(const fs::path& filepath)
{
    return "#include \"" + filepath.string() + "\"";
}

void Parser::ProcessFile()
{
    std::string          path_arg    = "-I" + Context.Parser.CurrentSource->Path.string();
    std::vector<cstring> arguments   = BuildArguments(path_arg);
    fs::path             outputPath  = BuildOutputPath(Context.Parser.CurrentSource->Path);
    std::string          fileContent = BuildFileContents(Context.Parser.CurrentSource->Path);

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
