//
// Created by bentoo on 25.03.18.
//

#ifndef ANNOTATIONGENERATOR_CODETEMPLATE_H
#define ANNOTATIONGENERATOR_CODETEMPLATE_H

#include "SourceFile.h"
#include "Utils/Filesystem.h"

#include <kainjow/mustache.hpp>
#include <memory>
#include <vector>

using TMustacheView = kainjow::mustache::mustache;
using TMustacheData = kainjow::mustache::data;

struct MustacheTemplate
{
    explicit MustacheTemplate(const fs::path& path)
    : Path(path)
    { }

    MustacheTemplate(const MustacheTemplate&) = delete;

    fs::path                       Path;
    std::unique_ptr<TMustacheView> View;
};

class SourcePattern
{
private:
    void LoadTemplate(std::unique_ptr<MustacheTemplate>& tmpl);

public:
    SourcePattern()
        : UseIncludes(false), RequireAnnotation(false)
    { }

    SourcePattern(const SourcePattern&) = delete;

    std::unique_ptr<MustacheTemplate>              MainTemplate;
    std::vector<std::unique_ptr<MustacheTemplate>> Templates;
    std::vector<std::unique_ptr<SourceFile>>       Sources;
    std::vector<std::string>                       Annotations;
    std::vector<std::string>                       Includes;
    std::vector<std::string>                       Directories;

    fs::path    OutputDir;
    std::string ClassOutName;
    std::string MainOutName;
    bool        UseIncludes;
    bool        RequireAnnotation;

    void LoadTemplates();
};

#endif //ANNOTATIONGENERATOR_CODETEMPLATE_H
