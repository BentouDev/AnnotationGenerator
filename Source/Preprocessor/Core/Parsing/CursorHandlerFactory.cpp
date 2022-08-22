//
// Created by bentoo on 22.04.18.
//

#include "CursorHandlerFactory.h"
#include "ParseContext.h"
#include "Visitor.h"
#include "Core/SourcePattern.h"
#include "Utils/Utils.h"

#include <iostream>

void CursorHandlerFactory::RegisterHandlers(const std::vector<std::pair<CXCursorKind, TCursorTypeHandler>>& handlers)
{
    for (auto& pair : handlers)
    {
        RegisterHandler(pair.first, pair.second);
    }
}

void CursorHandlerFactory::RegisterHandler(CXCursorKind cursorKind, const TCursorTypeHandler& handler)
{
    Data[cursorKind] = handler;
}

auto CursorHandlerFactory::Handle(ParseContext& context, CXCursor cursor, CXCursorKind kind, Visitor& visitor) -> TCursorResolveResult
{
    if (auto itr = Data.find(kind); itr != Data.end())
    {
        return (itr->second)(context, cursor, visitor);
    }

    return { false, nullptr };
}

namespace Handlers
{
    bool IsForwardDeclaration(CXCursor cursor)
    {
        auto definition = clang_getCursorDefinition(cursor);

        // If the definition is null, then there is no definition in this translation
        // unit, so this cursor must be a forward declaration.
        if (clang_equalCursors(definition, clang_getNullCursor()))
            return true;

        // If there is a definition, then the forward declaration and the definition
        // are in the same translation unit. This cursor is the forward declaration if
        // it is _not_ the definition.
        return !clang_equalCursors(cursor, definition);
    }

    auto HandleVarDecl(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
//        std::string name  = visitor.GetCursorSpelling(cursor);

        auto        type = clang_getCursorType(cursor);
        std::string name = visitor.GetTypeSpelling(type);
        auto        annot = std::find_if(context.CurrentPattern->Annotations.begin(),
                                         context.CurrentPattern->Annotations.end(),

            [&](const std::string& annot_name) {
                return annot_name.compare(name) == 0;
            }
        );

        if (annot != context.CurrentPattern->Annotations.end())
        {
            context.PushFactory(context.AnnotFactory);

            return { true, nullptr };
        }

        return { false, nullptr };
    }

    auto SkipRecurse(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);
        UNUSED(cursor);
        UNUSED(visitor);

        return { true, nullptr };
    }

    auto HandleNamespace(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);
        UNUSED(cursor);
        UNUSED(visitor);

        return { true, nullptr };
    }

    auto HandleEnum(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);
        UNUSED(cursor);
        UNUSED(visitor);

        return { false, nullptr };
    }

    auto HandleTypeAlias(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        std::string name       = visitor.GetCursorSpelling(cursor);
        auto        type       = clang_getCursorType(cursor);
        auto        canon      = clang_getCanonicalType(type);
        auto        canon_ref  = clang_getTypeSpelling(canon);
        std::string canon_name = clang_getCString(canon_ref);

        if (canon_name.empty())
        {
            std::stringstream ss;
            ss << visitor.BuildScopeNamePrefix() << name;

            canon_name = ss.str();
        }

        if (auto itr = context.Classes.find(canon_name); itr == context.Classes.end())
        {
            auto clazz              = std::make_shared<ClassInfo>(name);
                 clazz->CanonName   = canon_name;
                 clazz->FromInclude = context.CurrentSource->Path.string();

            clazz->Annotations.reserve(clazz->Annotations.size() + visitor.PreAnnotations.size());
            clazz->Annotations.insert (clazz->Annotations.end(), visitor.PreAnnotations.begin(), visitor.PreAnnotations.end());
            visitor.PreAnnotations.clear();

            clazz->HasAnnotation |= visitor.HasPreAnnotation;
            visitor.HasPreAnnotation = false;

            context.Classes.insert(std::make_pair(canon_name, clazz));

            return { true, clazz };
        }

        return { false, nullptr };
    }

    auto HandleType(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        if (IsForwardDeclaration(cursor))
            return { false, nullptr };

        std::string name       = visitor.GetCursorSpelling(cursor);
        auto        type       = clang_getCursorType(cursor);
        auto        canon_ref  = clang_getTypeSpelling(type);
        std::string canon_name = clang_getCString(canon_ref);

        if (canon_name.empty())
        {
            std::stringstream ss;
            ss << visitor.BuildScopeNamePrefix() << name;

            canon_name = ss.str();
        }

        if (auto itr = context.Classes.find(canon_name); itr == context.Classes.end())
        {
            auto clazz              = std::make_shared<ClassInfo>(name);
                 clazz->CanonName   = canon_name;
                 clazz->FromInclude = context.CurrentSource->Path.string();

            clazz->Annotations.reserve(clazz->Annotations.size() + visitor.PreAnnotations.size());
            clazz->Annotations.insert (clazz->Annotations.end(), visitor.PreAnnotations.begin(), visitor.PreAnnotations.end());
            visitor.PreAnnotations.clear();
            
            clazz->HasAnnotation |= visitor.HasPreAnnotation;
            visitor.HasPreAnnotation = false;

            context.Classes.insert(std::make_pair(canon_name, clazz));

            return { true, clazz };
        }

        return { false, nullptr };
    }

    auto HandleField(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);

        auto type  = clang_getCursorType(cursor);
        auto field = visitor.GetScope().top()->asType()->Fields.emplace_back
        (
                std::make_shared<FieldInfo>(visitor.GetCursorSpelling(cursor),
                                            context.GetTypeInfo(visitor.GetTypeSpelling(type)))
        );

        return { false, field };
    }

    auto HandleMethod(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        auto type        = clang_getCursorType(cursor);
        auto return_type = clang_getResultType(type);
        int  arg_result = clang_Cursor_getNumArguments(cursor);
        unsigned arg_count = 0U;

        if (arg_result >= 0)
        {
            arg_count = (unsigned int) arg_result;
        }

        std::shared_ptr<MethodInfo> method = visitor.GetScope().top()->asType()->Methods.emplace_back(std::make_shared<MethodInfo>
        (
             visitor.GetCursorSpelling(cursor),
             context.GetTypeInfo(visitor.GetTypeSpelling(return_type))
        ));

        for (unsigned int i = 0; i < arg_count; i++)
        {
            auto arg_cursor  = clang_Cursor_getArgument(cursor, i);
            auto cursor_type = clang_getCursorType(arg_cursor);

            method->Parameters.emplace_back(context.GetTypeInfo(visitor.GetTypeSpelling(cursor_type)));
        }

        return { false, method };
    }

    auto HandleAnnotation(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);

        std::string name = visitor.GetCursorSpelling(cursor);

        std::shared_ptr<ClassInfo> clazz = visitor.GetScope().top()->asType();
        clazz->Annotations.emplace_back(std::move(name));
        clazz->HasAnnotation = true;

        return { false, nullptr };
    }
    
    auto HandlePreAnnot(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);

        std::string declaration = visitor.GetCursorSpelling(cursor);
        std::string name        = declaration.substr(1, declaration.size() - 2);

        visitor.PreAnnotations.emplace_back(std::move(name));
        visitor.HasPreAnnotation = true;

        context.PopFactory(context.AnnotFactory);

        return { false, nullptr };
    }
}