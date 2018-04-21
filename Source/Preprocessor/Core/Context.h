//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_OPTIONS_H
#define ANNOTATIONGENERATOR_OPTIONS_H

#include <vector>
#include <memory>
#include "SourcePattern.h"
#include "ReflectionInfo.h"

namespace Data
{
    class Context
    {
    public:
        std::vector<std::unique_ptr<SourcePattern>> Templates;

        struct {
            SourceFile*                             CurrentSource;
            std::vector<std::shared_ptr<ClassInfo>> Classes;
            std::vector<std::string>                Includes;
        } Parser;

        struct {
            SourceFile*    CurrentSource;
            SourcePattern* CurrentPattern;
        } Generator;

        Context()
            : Parser { nullptr }
            , Generator { nullptr, nullptr }
        { }
    };
}

#endif //ANNOTATIONGENERATOR_OPTIONS_H
