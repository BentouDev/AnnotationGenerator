//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H
#define ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H

#include "Typedefs.h"

#include <clang-c/Index.h>
#include <map>

class ParseContext;
class Visitor;

class CursorHandlerFactory
{
    std::map<CXCursorKind, TCursorTypeHandler> Data;

public:
    void RegisterHandlers(const std::vector<std::pair<CXCursorKind, TCursorTypeHandler>>& handlers);
    void RegisterHandler(CXCursorKind cursorKind, const TCursorTypeHandler& handler);
    auto Handle(ParseContext& context, CXCursor cursor, CXCursorKind kind, Visitor& visitor) -> TCursorResolveResult;
};

namespace Handlers
{
    bool IsForwardDeclaration(CXCursor cursor);

    auto SkipRecurse     (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    void HandleAttributes(ParseContext& context, CXCursor cursor, std::shared_ptr<TypeInfo> type);
    auto HandlePreAnnot  (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleVarDecl   (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleNamespace (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleType      (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleTypeAlias (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleEnum      (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleEnumValue (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleField     (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleMethod    (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleAnnotation(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
    auto HandleBaseClass (ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult;
}

#endif //ANNOTATIONGENERATOR_CURSORHANDLERFACTORY_H
