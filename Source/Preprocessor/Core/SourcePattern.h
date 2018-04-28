//
// Created by bentoo on 25.03.18.
//

#ifndef ANNOTATIONGENERATOR_CODETEMPLATE_H
#define ANNOTATIONGENERATOR_CODETEMPLATE_H

#include <memory>
#include <vector>
#include <mustache.hpp>
#include "SourceFile.h"
#include "../Utils/Filesystem.h"

using TMustacheView = kainjow::mustache::mustache;
using TMustacheData = kainjow::mustache::data;

struct MustacheTemplate
{
    explicit MustacheTemplate(const fs::path& path)
    : Path(path)
    { }

    fs::path                       Path;
    std::unique_ptr<TMustacheView> View;
};

class SourcePattern
{
public:
    std::vector<std::unique_ptr<MustacheTemplate>> Templates;
    std::vector<std::unique_ptr<SourceFile>>       Sources;

    std::string Annotation;
    std::string OutName;

    void LoadTemplate();
};

#endif //ANNOTATIONGENERATOR_CODETEMPLATE_H
