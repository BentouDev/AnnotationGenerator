//
// Created by bentoo on 01.04.18.
//

#include <iostream>
#include "Visitor.h"
#include "Context.h"

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
{
    // Move this to factory
    auto register_type = [this](CXCursor cursor) { return TryRegisterType(cursor); };
    auto register_enum = [this](CXCursor cursor) { return TryRegisterEnum(cursor); };

    GlobalScopeHandler[CXCursor_EnumDecl]   = register_enum;
    GlobalScopeHandler[CXCursor_ClassDecl]  = register_type;
    GlobalScopeHandler[CXCursor_StructDecl] = register_type;
    GlobalScopeHandler[CXCursor_Namespace]  = [this](CXCursor) -> TCursorResolveResult { return {true, nullptr}; };

    TypeScopeHandler[CXCursor_EnumDecl]     = register_enum;
    TypeScopeHandler[CXCursor_ClassDecl]    = register_type;
    TypeScopeHandler[CXCursor_StructDecl]   = register_type;
    TypeScopeHandler[CXCursor_FieldDecl]    = [this](CXCursor cursor) { return TryRegisterField(cursor); };
    TypeScopeHandler[CXCursor_FunctionDecl] = [this](CXCursor cursor) { return TryRegisterMethod(cursor); };
}

auto Visitor::TryRegisterEnum(CXCursor cursor) -> TCursorResolveResult
{
    return {false, nullptr};
}

auto Visitor::TryRegisterType(CXCursor cursor) -> TCursorResolveResult
{
    auto& clazz = Context.Parser.Classes.emplace_back(std::make_shared<ClassInfo>());
    clazz->Name = GetCursorSpelling(cursor);

    return {true, clazz};
}

auto Visitor::TryRegisterField(CXCursor cursor) -> TCursorResolveResult
{
    auto field = Scope.top()->asType()->Fields.emplace_back(std::make_shared<FieldInfo>());
    field->Name = GetCursorSpelling(cursor);

    return {false, field};
}

auto Visitor::TryRegisterMethod(CXCursor cursor) -> TCursorResolveResult
{
    auto method = Context.Parser.Classes.back()->Methods.emplace_back(std::make_shared<MethodInfo>());
    method->Name = GetCursorSpelling(cursor);

    return {false, method};
}

auto Visitor::TryAssignAnnotation(CXCursor cursor) -> TCursorResolveResult
{
    return {false, nullptr};
}

auto Visitor::ResolveCursor(CXCursor cursor, CXCursorKind cursorKind) -> TCursorResolveResult
{
    bool  is_in_type_scope = !Scope.empty() && Scope.top()->isType;
    auto& scope_dictionary = is_in_type_scope ? TypeScopeHandler : GlobalScopeHandler;

    if (auto itr = scope_dictionary.find(cursorKind); itr != scope_dictionary.end())
    {
        return (itr->second)(cursor);
    }

    return {false, nullptr};
}

std::string Visitor::GetCursorKindName(CXCursorKind cursorKind)
{
    CXString kindName  = clang_getCursorKindSpelling(cursorKind);
    std::string result = clang_getCString(kindName);

    clang_disposeString(kindName);
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

    if (!fs::equivalent(source, Context.Parser.CurrentSource->Path))
    {
        return CXChildVisit_Continue;
    }

    CXCursorKind cursorKind = clang_getCursorKind(cursor);
    std::string  cursorName = GetCursorSpelling(cursor);
    auto         current_level = (unsigned int) Scope.size();

    std::cout << std::string(current_level, '\t')
              << " " << GetCursorKindName(cursorKind)
              << " (" << cursorName
              << ")" << std::endl;

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
