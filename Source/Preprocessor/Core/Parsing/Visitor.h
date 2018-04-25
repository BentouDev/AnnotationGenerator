//
// Created by bentoo on 01.04.18.
//

#ifndef ANNOTATIONGENERATOR_VISITOR_H
#define ANNOTATIONGENERATOR_VISITOR_H

#include <clang-c/Index.h>
#include <map>
#include <stack>
#include <functional>

#include "Typedefs.h"
#include "../Context.h"
#include "../../Utils/Utils.h"

class Visitor
{
    static CXChildVisitResult routine_wrapper(CXCursor cursor, CXCursor /* parent */, CXClientData data);

    struct ScopeInfo
    {
        ScopeInfo(std::shared_ptr<MetaInfo> obj, const std::string& name, CXCursor cursor);
        auto asType() const -> std::shared_ptr<ClassInfo>;

        std::shared_ptr<MetaInfo> ReflectionObj;
        std::string               Name;
        CXCursor                  Cursor;
        bool                      isType;

    };

    using TScope = Utils::IterableStack<std::unique_ptr<ScopeInfo>>;

    fs::path    GetCursorSourcePath(CXCursor param);
    std::string GetCursorKindName(CXCursorKind cursorKind);

    TCursorResolveResult ResolveCursor(CXCursor cursor, CXCursorKind kind);
    CXChildVisitResult   RoutineStep(CXCursor cursor, CXCursor parent);

    Data::Context& Context;

    TScope Scope;

public:
    explicit Visitor(Data::Context& context);

    void VisitChildren(CXCursor param);

    std::string GetCursorSpelling(CXCursor cursor);
    std::string GetTypeSpelling(CXType cursor);
    std::string BuildScopeNamePrefix() const;
    TScope&     GetScope() { return Scope; }
};


#endif //ANNOTATIONGENERATOR_VISITOR_H
