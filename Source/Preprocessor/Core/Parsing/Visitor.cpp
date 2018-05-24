//
// Created by bentoo on 01.04.18.
//

#include <iostream>
#include "Visitor.h"
#include "../Context.h"

Visitor::ScopeInfo::ScopeInfo(std::shared_ptr<MetaInfo> obj, const std::string& name, CXCursor cursor)
: ReflectionObj(obj), Name(name), Cursor(cursor)
{
    auto ptr = std::dynamic_pointer_cast<ClassInfo>(ReflectionObj);
    isType   = (bool) ptr;
}

std::shared_ptr<ClassInfo> Visitor::ScopeInfo::asType() const
{
    auto ptr = std::dynamic_pointer_cast<ClassInfo>(ReflectionObj);
    return ptr;
}

Visitor::Visitor(Data::Context& context)
: Context(context)
{ }

std::string Visitor::BuildScopeNamePrefix() const
{
    if (Scope.empty())
        return std::string();

    std::stringstream ss;
    for (auto& scope : Scope)
    {
        ss << scope->Name << "::";
    }

    return ss.str();
}

auto Visitor::ResolveCursor(CXCursor cursor, CXCursorKind cursorKind) -> TCursorResolveResult
{
    bool  is_in_type_scope = !Scope.empty() && Scope.top()->isType;
    auto& scope_handler = is_in_type_scope ? Context.Parser.TypeFactory : Context.Parser.GlobalFactory;

    return scope_handler->Handle(Context.Parser, cursor, cursorKind, *this);
}

std::string Visitor::GetCursorKindName(CXCursorKind cursorKind)
{
    CXString kindName  = clang_getCursorKindSpelling(cursorKind);
    std::string result = clang_getCString(kindName);

    clang_disposeString(kindName);
    return result;
}

std::string Visitor::GetTypeSpelling(CXType type)
{
    CXString cursorSpelling = clang_getTypeSpelling(type);
    std::string result      = clang_getCString(cursorSpelling);

    clang_disposeString(cursorSpelling);
    return result;
}

std::string Visitor::GetCursorSpelling(CXCursor cursor)
{
    CXString cursorSpelling = clang_getCursorSpelling(cursor);
    std::string result      = clang_getCString(cursorSpelling);

    clang_disposeString(cursorSpelling);
    return result;
}

fs::path Visitor::GetCursorSourcePath(CXCursor cursor)
{
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile           source{};
    fs::path         source_path;

    clang_getExpansionLocation(location, &source, nullptr, nullptr, nullptr);

    if (source)
        source_path += reinterpret_cast<const char*>(clang_getFileName(source).data);

    return source_path;
}

CXChildVisitResult Visitor::routine_wrapper(CXCursor cursor, CXCursor parent, CXClientData data)
{
    return reinterpret_cast<Visitor*>(data)->RoutineStep(cursor, parent);
}

CXChildVisitResult Visitor::RoutineStep(CXCursor cursor, CXCursor /* parent */)
{
    fs::path source = GetCursorSourcePath(cursor);

    if (!fs::equivalent(source, Context.Parser.CurrentSource->Path)
    &&  source.string() != Context.Parser.CurrentUnitName)
    {
        return CXChildVisit_Continue;
    }

    CXCursorKind cursorKind = clang_getCursorKind(cursor);
    std::string  cursorName = GetCursorSpelling(cursor);
//    auto         current_level = (unsigned int) Scope.size();

//    std::cout << std::string(current_level, '\t')
//              << " " << GetCursorKindName(cursorKind)
//              << " (" << cursorName
//              << ")" << std::endl;

    auto [is_scope, info] = ResolveCursor(cursor, cursorKind);
    if (is_scope)
    {
        Scope.emplace(std::make_unique<ScopeInfo>(info, cursorName, cursor));
        {
            VisitChildren(cursor);
        }
        Scope.pop();
    }

    return CXChildVisit_Continue;
}

void Visitor::VisitChildren(CXCursor root)
{
    clang_visitChildren(root, Visitor::routine_wrapper, this);
}
