//
// Created by bentoo on 22.04.18.
//

#include "ParseContext.h"

void ParseContext::PushFactory(std::unique_ptr<CursorHandlerFactory>& factory)
{
    if (ForcedFactory == nullptr)
        ForcedFactory = factory.get();
}

void ParseContext::PopFactory (std::unique_ptr<CursorHandlerFactory>& factory)
{
    if (ForcedFactory == factory.get())
        ForcedFactory = nullptr;
}

std::shared_ptr<TypeInfo> ParseContext::GetTypeInfo(const std::string& type_name)
{
    if (auto itr = Classes.find(type_name); itr != Classes.end())
    {
        return std::dynamic_pointer_cast<TypeInfo>(itr->second);
    }

    // Stub with just name
    auto info = std::make_shared<TypeInfo>(type_name);

    return info;
}
