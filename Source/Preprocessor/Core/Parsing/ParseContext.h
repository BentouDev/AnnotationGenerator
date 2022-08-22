//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_PARSECONTEXT_H
#define ANNOTATIONGENERATOR_PARSECONTEXT_H

#include "CursorHandlerFactory.h"
#include "Core/SourceFile.h"

#include <ReflectionInfo.h>

using TClassMap = std::map<std::string, std::shared_ptr<ClassInfo>>;

class SourcePattern;
class ParseContext
{
public:
    ParseContext()
        : CurrentSource { nullptr }, ForcedFactory { nullptr }
    { }

    ParseContext(const ParseContext&) = delete;

    std::string                             CurrentUnitName;
    SourceFile*                             CurrentSource;
    SourcePattern*                          CurrentPattern;
    TClassMap                               Classes;
    std::vector<std::string>                Includes;

    std::unique_ptr<CursorHandlerFactory>   GlobalFactory;
    std::unique_ptr<CursorHandlerFactory>   TypeFactory;
    std::unique_ptr<CursorHandlerFactory>   AnnotFactory;
    CursorHandlerFactory*                   ForcedFactory;

    bool                                    UseIncludes;

    void PushFactory(std::unique_ptr<CursorHandlerFactory>& factory);
    void PopFactory (std::unique_ptr<CursorHandlerFactory>& factory);

    std::shared_ptr<TypeInfo> GetTypeInfo(const std::string& type_name);
};

#endif //ANNOTATIONGENERATOR_PARSECONTEXT_H
