//
// Created by bentoo on 01.04.18.
//

#ifndef ANNOTATIONGENERATOR_VISITOR_H
#define ANNOTATIONGENERATOR_VISITOR_H

#include "Typedefs.h"
#include "Core/Context.h"
#include "Utils/Utils.h"

#include <clang-c/Index.h>
#include <map>
#include <stack>
#include <functional>

class Visitor
{
    static CXChildVisitResult routine_wrapper(CXCursor cursor, CXCursor /* parent */, CXClientData data) noexcept;

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

    fs::path    GetCursorSourcePath(CXCursor param) noexcept;

    TCursorResolveResult ResolveCursor(CXCursor cursor, CXCursorKind kind);
    CXChildVisitResult   RoutineStep(CXCursor cursor, CXCursor parent) noexcept;

    Data::Context& Context;

    TScope Scope;

public:
    explicit Visitor(Data::Context& context);

    void VisitChildren(CXCursor param) noexcept;

    std::vector<std::string> PreAnnotations;
    bool HasPreAnnotation;

    std::string GetCursorKindName(CXCursorKind cursorKind) noexcept;
    std::string GetCursorSpelling(CXCursor cursor) noexcept;
    std::string GetTypeSpelling(CXType cursor) noexcept;
    std::string BuildScopeNamePrefix() const;
    TScope&     GetScope() { return Scope; }
};


#endif //ANNOTATIONGENERATOR_VISITOR_H
