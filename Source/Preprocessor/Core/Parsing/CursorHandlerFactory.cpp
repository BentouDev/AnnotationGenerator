//
// Created by bentoo on 22.04.18.
//

#include "CursorHandlerFactory.h"
#include "ParseContext.h"
#include "Visitor.h"

CursorHandlerFactory::CursorHandlerFactory()
{ }

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
        return { true, nullptr };
    }

    auto HandleEnum(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        return { false, nullptr };
    }

    auto HandleType(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        auto& clazz = context.Classes.emplace_back(std::make_shared<ClassInfo>());
        clazz->Name = visitor.GetCursorSpelling(cursor);

        return { true, clazz };
    }

    auto HandleField(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        auto field  = visitor.GetScope().top()->asType()->Fields.emplace_back(std::make_shared<FieldInfo>());
        field->Name = visitor.GetCursorSpelling(cursor);

        return { false, field };
    }

    auto HandleMethod(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        auto method  = context.Classes.back()->Methods.emplace_back(std::make_shared<MethodInfo>());
        method->Name = visitor.GetCursorSpelling(cursor);

        return { false, method };
    }

    auto HandleAnnotation(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        return { false, nullptr };
    }
}