//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_PARSECONTEXT_H
#define ANNOTATIONGENERATOR_PARSECONTEXT_H

#include "CursorHandlerFactory.h"
#include "Core/SourceFile.h"

#include <clang-c/Index.h>

#include <ReflectionInfo.h>

using TClassMap = std::map<std::string, std::shared_ptr<ClassInfo>>;
using TEnumMap = std::map<std::string, std::shared_ptr<EnumInfo>>;

class SourcePattern;
class ParseContext
{
public:
    ParseContext()
        : CurrentSource { nullptr }, ForcedFactory { nullptr }, TranslationUnit { nullptr }
    { }

    ParseContext(const ParseContext&) = delete;

    std::string                             CurrentUnitName;
    SourceFile*                             CurrentSource;
    SourcePattern*                          CurrentPattern;
    TClassMap                               Classes;
    TEnumMap                                Enums;
    std::vector<std::string>                Includes;

    std::unique_ptr<CursorHandlerFactory>   GlobalFactory;
    std::unique_ptr<CursorHandlerFactory>   TypeFactory;
    std::unique_ptr<CursorHandlerFactory>   AnnotFactory;
    CursorHandlerFactory*                   ForcedFactory;

    CXTranslationUnit*                      TranslationUnit;
    CXFile                                  FileHandle;

    bool                                    IncludeSourceHeader;

    void PushFactory(std::unique_ptr<CursorHandlerFactory>& factory);
    void PopFactory (std::unique_ptr<CursorHandlerFactory>& factory);

    std::shared_ptr<TypeInfo> GetTypeInfo(const std::string& type_name);
};

#endif //ANNOTATIONGENERATOR_PARSECONTEXT_H
