//
// Created by bentoo on 25.03.18.
//

#include <fstream>
#include <iostream>
#include "SourcePattern.h"

void SourcePattern::LoadTemplate()
{
    for (auto& templ : Templates)
    {
        if (fs::exists(templ->Path))
        {
            std::fstream file(templ->Path);
            std::string  mustache_view;

            mustache_view.reserve(fs::file_size(templ->Path));
            mustache_view.assign((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());

            templ->View = std::make_unique<TMustacheView>(mustache_view);
        }
        else
        {
            std::cerr << "Unable to open mustache file " << templ->Path << "." << std::endl;
        }
    }
}
