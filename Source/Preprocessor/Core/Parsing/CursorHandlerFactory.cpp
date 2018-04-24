//
// Created by bentoo on 22.04.18.
//

#include "CursorHandlerFactory.h"
#include "ParseContext.h"
#include "Visitor.h"
#include "../../Utils/Utils.h"

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
            canon_name = name;
            std::cerr << "Warning: unable to get canon name for type '" << name << "'!"
                      << std::endl << "\t" << context.CurrentSource->Path << std::endl;
        }

        if (auto itr = context.Classes.find(canon_name); itr == context.Classes.end())
        {
            auto clazz            = std::make_shared<ClassInfo>();
                 clazz->Name      = name;
                 clazz->CanonName = canon_name;

            context.Classes.insert(std::make_pair(canon_name, clazz));

            return { true, clazz };
        }

        return { false, nullptr };
    }

    auto HandleType(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        std::string name       = visitor.GetCursorSpelling(cursor);
        auto        type       = clang_getCursorType(cursor);
        auto        canon_ref  = clang_getTypeSpelling(type);
        std::string canon_name = clang_getCString(canon_ref);

        if (canon_name.empty())
        {
            canon_name = name;
            std::cerr << "Warning: unable to get canon name for type '" << name << "'!"
                      << std::endl << "\t" << context.CurrentSource->Path << std::endl;
        }

        if (auto itr = context.Classes.find(canon_name); itr == context.Classes.end())
        {
            auto clazz       = std::make_shared<ClassInfo>();
            clazz->Name      = name;
            clazz->CanonName = canon_name;

            context.Classes.insert(std::make_pair(canon_name, clazz));

            return { true, clazz };
        }

        return { false, nullptr };
    }

    auto HandleField(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);

        auto field  = visitor.GetScope().top()->asType()->Fields.emplace_back(std::make_shared<FieldInfo>());
        field->Name = visitor.GetCursorSpelling(cursor);

        return { false, field };
    }

    auto HandleMethod(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        auto method  = visitor.GetScope().top()->asType()->Methods.emplace_back(std::make_shared<MethodInfo>());
        method->Name = visitor.GetCursorSpelling(cursor);

        return { false, method };
    }

    auto HandleAnnotation(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);
        UNUSED(cursor);
        UNUSED(visitor);

        return { false, nullptr };
    }
}