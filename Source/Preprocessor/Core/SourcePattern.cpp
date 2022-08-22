//
// Created by bentoo on 25.03.18.
//

#include "SourcePattern.h"

#include <fstream>
#include <iostream>

void SourcePattern::LoadTemplates()
{
    for (auto& templ : Templates)
    {
        LoadTemplate(templ);
    }

    LoadTemplate(MainTemplate);
}

void SourcePattern::LoadTemplate(std::unique_ptr<MustacheTemplate>& templ)
{
    if (fs::exists(templ->Path))
    {
        std::fstream file(templ->Path);
        std::string  mustache_view;

        mustache_view.reserve((unsigned int) fs::file_size(templ->Path));
        mustache_view.assign((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        templ->View = std::make_unique<TMustacheView>(mustache_view);
    }
    else
    {
        std::cerr << "Unable to open mustache file " << templ->Path << "." << std::endl;
    }
}