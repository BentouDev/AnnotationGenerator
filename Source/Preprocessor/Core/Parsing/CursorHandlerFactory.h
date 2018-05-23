//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H
#define ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H

#include <clang-c/Index.h>
#include <map>

#include "Typedefs.h"

class ParseContext;
class Visitor;

class CursorHandlerFactory
{
    std::map<CXCursorKind, TCursorTypeHandler> Data;

    bool IsForwardDeclaration(CXCursor cursor) const;

public:
    void RegisterHandlers(const std::vector<std::pair<CXCursorKind, TCursorTypeHandler>>& handlers);
    void RegisterHandler(CXCursorKind cursorKind, const TCursorTypeHandler& handler);
    auto Handle(ParseContext& context, CXCursor cursor, CXCursorKind kind, Visitor& visitor) -> TCursorResolveResult;
};

namespace Handlers
{
    auto HandleNamespace (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleType      (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleTypeAlias (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleEnum      (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleField     (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleMethod    (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleAnnotation(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
}

#endif //ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H
