//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_OPTIONS_H
#define ANNOTATIONGENERATOR_OPTIONS_H

#include <vector>
#include <memory>
#include "SourcePattern.h"
#include "ReflectionInfo.h"
#include "Parsing/ParseContext.h"

namespace Data
{
    class Context
    {
    public:
        std::vector<std::unique_ptr<SourcePattern>> Templates;

        ParseContext Parser;

        struct {
            SourcePattern* CurrentPattern;
            fs::path       OutputDirectory;
        } Generator;

        Context()
            : Generator { nullptr }
        { }
    };
}

#endif //ANNOTATIONGENERATOR_OPTIONS_H
