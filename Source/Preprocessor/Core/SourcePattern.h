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
        : IncludeSourceHeader(false)
    { }

    SourcePattern(const SourcePattern&) = delete;

    std::unique_ptr<MustacheTemplate>              MainTemplate;
    std::vector<std::unique_ptr<MustacheTemplate>> HeaderTemplates;
    std::vector<std::unique_ptr<MustacheTemplate>> ClassTemplates;
    std::vector<std::unique_ptr<MustacheTemplate>> EnumTemplates;
    std::vector<std::unique_ptr<SourceFile>>       Sources;
    std::vector<std::string>                       EnablePreAnnotationFrom;
    std::vector<std::string>                       InjectIncludes;
    std::vector<std::string>                       IncludeDirectories;

    fs::path    OutputDir;
    std::string MainOutName;
    std::string HeaderOutName;
    std::string ClassOutName;
    std::string EnumOutName;
    bool        IncludeSourceHeader;
    std::vector<std::string> RequireAnnotation;

    void LoadTemplates();
};

#endif //ANNOTATIONGENERATOR_CODETEMPLATE_H
