//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_PARSECONTEXT_H
#define ANNOTATIONGENERATOR_PARSECONTEXT_H

#include <ReflectionInfo.h>
#include "CursorHandlerFactory.h"
#include "../SourceFile.h"

using TClassMap = std::map<std::string, std::shared_ptr<ClassInfo>>;

class ParseContext
{
public:
    SourceFile*                             CurrentSource;
    TClassMap                               Classes;
    std::vector<std::string>                Includes;

    std::unique_ptr<CursorHandlerFactory>   GlobalFactory;
    std::unique_ptr<CursorHandlerFactory>   TypeFactory;
};

#endif //ANNOTATIONGENERATOR_PARSECONTEXT_H
