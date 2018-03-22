//
// Created by bentoo on 10.09.17.
//

#include "Preprocessor.h"
#include "../Utils/Utils.h"
#include <clang-c/Index.h>
#include <cstring>
#include <experimental/filesystem>
#include <fstream>

namespace fs = std::experimental::filesystem;

Preprocessor::Preprocessor(const Context& options)
: CurrentContext(options)
{ }

int Preprocessor::Run()
{
    for (const std::string& filepath : CurrentContext.FilePaths)
    {
        ProcessFile(filepath);
    }

    return 0;
}

void Preprocessor::ProcessFile(const std::string& filename)
{
    fs::path filepath;
    filepath += filename;

    char const * args[] =
    {
        "-std=c++11",
        "-Wmicrosoft",
        "-Wunknown-pragmas",
        "-D_DEBUG=1",
        ("-I" + filepath.string()).c_str()
    };

    fs::path outputPath = "gen_" + filepath.stem().string() + ".cpp";

    CXUnsavedFile dummyFile{};
    dummyFile.Filename = outputPath.c_str();
    dummyFile.Contents = ("#include \"" + filepath.string() + "\"").c_str();
    dummyFile.Length = std::strlen( dummyFile.Contents );

    CXIndex CIdx = clang_createIndex(1, 1);
    CXTranslationUnit unit = clang_parseTranslationUnit
    (
        CIdx, outputPath.c_str(),
        args,
        countof(args),
        &dummyFile,
        1,
        CXTranslationUnit_None
    );

    // output this new file into the same directory, so user can just #include "gen_header.h"
    // Or... do we actually need this?
    // No!
    // We can output to cpp and generate func that will load whole reflection at once!
    // And then just #include "reflection.h" and Meta::LoadReflectionData();
    // We should call getOutputPath(path) so we can have a different folder for generated cpp's
}
