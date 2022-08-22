//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_OPTIONS_H
#define ANNOTATIONGENERATOR_OPTIONS_H

#include "SourcePattern.h"
#include "ReflectionInfo.h"
#include "Core/Parsing/ParseContext.h"

#include <vector>
#include <memory>

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

        Context(const Context&) = delete;
        Context(Context&&) = delete;
    };
}

#endif //ANNOTATIONGENERATOR_OPTIONS_H
