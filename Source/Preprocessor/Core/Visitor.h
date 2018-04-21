//
// Created by bentoo on 01.04.18.
//

#ifndef ANNOTATIONGENERATOR_VISITOR_H
#define ANNOTATIONGENERATOR_VISITOR_H

#include <clang-c/Index.h>
#include <map>
#include <stack>
#include <functional>
#include "Context.h"

class Visitor
{
    static CXChildVisitResult routine_wrapper(CXCursor cursor, CXCursor /* parent */, CXClientData data);
    using TCursorResolveResult = std::pair<bool, std::shared_ptr<MetaInfo>>;
    using TCursorTypeHandler   = std::function<TCursorResolveResult(CXCursor)>;

    struct ScopeInfo
    {
        ScopeInfo(std::shared_ptr<MetaInfo> obj, const std::string& name, CXCursor cursor);
        auto asType() const -> std::shared_ptr<ClassInfo>;

        std::shared_ptr<MetaInfo> ReflectionObj;
        std::string               Name;
        CXCursor                  Cursor;
        bool                      isType;

    };

    fs::path    GetCursorSourcePath(CXCursor param);
    std::string GetCursorKindName(CXCursorKind cursorKind);
    std::string GetCursorSpelling(CXCursor cursor);

    auto TryRegisterType(CXCursor cursor) -> TCursorResolveResult;
    auto TryRegisterEnum(CXCursor cursor) -> TCursorResolveResult;
    auto TryRegisterField(CXCursor cursor) -> TCursorResolveResult;
    auto TryRegisterMethod(CXCursor cursor) -> TCursorResolveResult;
    auto TryAssignAnnotation(CXCursor cursor) -> TCursorResolveResult;

    TCursorResolveResult ResolveCursor(CXCursor cursor, CXCursorKind kind);
    CXChildVisitResult   RoutineStep(CXCursor cursor, CXCursor parent);

    Data::Context& Context;

    std::stack<std::unique_ptr<ScopeInfo>> Scope;
    std::map<CXCursorKind, TCursorTypeHandler> TypeScopeHandler;
    std::map<CXCursorKind, TCursorTypeHandler> GlobalScopeHandler;

public:
    explicit Visitor(Data::Context& context);

    void VisitChildren(CXCursor param);
};


#endif //ANNOTATIONGENERATOR_VISITOR_H
