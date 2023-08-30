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
        auto        type = clang_getCursorType(cursor);
        std::string name = visitor.GetTypeSpelling(type);
        auto        annot = std::find_if
        (
            context.CurrentPattern->EnablePreAnnotationFrom.begin(),
            context.CurrentPattern->EnablePreAnnotationFrom.end(),
        [&](const std::string& annot_name)
            {
                return annot_name.compare(name) == 0;
            }
        );

        if (annot != context.CurrentPattern->EnablePreAnnotationFrom.end())
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
        std::string name = visitor.GetCursorSpelling(cursor);
        auto        type = clang_getCursorType(cursor);
        auto        canon = clang_getCanonicalType(type);
        auto        canon_ref = clang_getTypeSpelling(canon);
        std::string canon_name = clang_getCString(canon_ref);

        if (canon_name.empty())
        {
            std::stringstream ss;
            ss << visitor.BuildScopeNamePrefix() << name;

            canon_name = ss.str();
        }

        if (auto itr = context.Enums.find(canon_name); itr == context.Enums.end())
        {
            auto enumz = std::make_shared<EnumInfo>(name);
            enumz->CanonName = canon_name;
            enumz->FromInclude = context.CurrentSource->Path.string();

            HandleAttributes(context, cursor, enumz);

            enumz->Annotations.reserve(enumz->Annotations.size() + visitor.PreAnnotations.size());
            enumz->Annotations.insert(enumz->Annotations.end(), visitor.PreAnnotations.begin(), visitor.PreAnnotations.end());
            visitor.PreAnnotations.clear();

            enumz->HasAnnotation |= visitor.HasPreAnnotation;
            visitor.HasPreAnnotation = false;

            context.Enums.insert(std::make_pair(canon_name, enumz));

            return { true, enumz };
        }

        return { false, nullptr };
    }

    auto HandleEnumValue(ParseContext& context, CXCursor cursor, Visitor& visitor) -> TCursorResolveResult
    {
        UNUSED(context);

        if (cursor.kind == CXCursor_EnumConstantDecl)
        {
            CXString spelling = clang_getCursorSpelling(cursor);

            visitor.GetScope().top()->asEnum()->Values.emplace_back(std::make_shared<EnumValueInfo>(
                clang_getCString(spelling),
                clang_getEnumConstantDeclValue(cursor)
            ));

            clang_disposeString(spelling);
        }

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

    void ParseAttributes(ParseContext& context, std::shared_ptr<TypeInfo> type, unsigned token_num, CXToken* tokens)
    {
        auto isDoubleToken = [&](unsigned idx, char symbol)
        {
            if (idx + 1 < token_num && clang_getTokenKind(tokens[idx + 1]) == CXToken_Punctuation)
            {
                CXString this_str_id = clang_getTokenSpelling(*context.TranslationUnit, tokens[idx]);
                CXString next_str_id = clang_getTokenSpelling(*context.TranslationUnit, tokens[idx + 1]);

                const char* first_c_str = clang_getCString(this_str_id);
                const char* second_c_str = clang_getCString(next_str_id);

                DISPOSE
                {
                    clang_disposeString(next_str_id);
                    clang_disposeString(this_str_id);
                };

                if (strcmp(first_c_str, second_c_str) != 0)
                {
                    return false;
                }

                return first_c_str[0] == symbol;
            }

            return false;
        };

        auto isNext = [&](unsigned idx, CXTokenKind kind)
        {
            if (idx + 1 < token_num && clang_getTokenKind(tokens[idx + 1]) == kind)
            {
                return true;
            }

            return false;
        };

        auto isCurrent = [&](unsigned idx, char symbol)
        {
            CXString this_str_id = clang_getTokenSpelling(*context.TranslationUnit, tokens[idx]);

            const char* str = clang_getCString(this_str_id);

            DISPOSE
            {
                clang_disposeString(this_str_id);
            };

            return str[0] == symbol;
        };

        auto addAtrribute = [&](unsigned from_token, unsigned to_token)
        {
            CXToken from = tokens[from_token];
            CXToken to = tokens[to_token];

            CXSourceLocation from_sloc = clang_getTokenLocation(*context.TranslationUnit, from);
            CXSourceLocation to_sloc = clang_getTokenLocation(*context.TranslationUnit, to);

            CXFile file_from, file_to;
            unsigned from_line, from_column, from_offset;
            unsigned to_line, to_column, to_offset;

            clang_getSpellingLocation(from_sloc, &file_from, &from_line, &from_column, &from_offset);
            clang_getSpellingLocation(to_sloc, &file_to, &to_line, &to_column, &to_offset);

            if (file_from == file_to)
            {
                size_t file_size;
                const char* file_buff = clang_getFileContents(*context.TranslationUnit, file_from, &file_size);

                std::string_view attribute_view{ &file_buff[from_offset], to_offset - from_offset };

                type->HasAnnotation |= true;
                type->Annotations.push_back(std::string(attribute_view));
            }
        };

        unsigned current_attribute_begin = 0;
        int attribute_brackets_depth = 0;
        int parthensis_depth = 0;

        for (unsigned idx = 0; idx < token_num; ++idx)
        {
            CXTokenKind token_kind = clang_getTokenKind(tokens[idx]);

            if (token_kind == CXToken_Punctuation)
            {
                if (parthensis_depth == 0 && isDoubleToken(idx, '['))
                {
                    ++idx;
                    ++attribute_brackets_depth;
                    current_attribute_begin = idx + 1;
                    continue;
                }
                else if (parthensis_depth == 0 && isDoubleToken(idx, ']'))
                {
                    --attribute_brackets_depth;

                    if (attribute_brackets_depth == 0)
                    {
                        addAtrribute(current_attribute_begin, idx);
                        current_attribute_begin = 0;
                    }

                    ++idx;

                    continue;
                }
                // Comma separated attributes
                else if (parthensis_depth == 0 && isCurrent(idx, ','))
                {
                    addAtrribute(current_attribute_begin, idx);

                    current_attribute_begin = idx + 1;
                }
                // Parameters of attribute
                else if (isCurrent(idx, '('))
                {
                    ++parthensis_depth;
                    continue;
                }
                else if (isCurrent(idx, ')'))
                {
                    --parthensis_depth;
                    continue;
                }
            }

            if (attribute_brackets_depth == 0 && token_kind == CXToken_Identifier)
            {
                CXString id_str = clang_getTokenSpelling(*context.TranslationUnit, tokens[idx + 1]);

                const char* name = clang_getCString(id_str);
                if (type->Name.compare(name) == 0)
                {
                    return;
                }

                DISPOSE
                {
                    clang_disposeString(id_str);
                };
            }
        }
    }

    void HandleAttributes(ParseContext& context, CXCursor cursor, std::shared_ptr<TypeInfo> type)
    {
        CXSourceRange cursor_extent = clang_getCursorExtent(cursor);

        unsigned start_line, start_column, start_offset;
        unsigned end_line, end_column, end_offset;

        clang_getExpansionLocation(clang_getRangeStart(cursor_extent), &context.FileHandle, &start_line, &start_column, &start_offset);
        clang_getExpansionLocation(clang_getRangeEnd(cursor_extent), &context.FileHandle, &end_line, &end_column, &end_offset);

        CXToken* sub_tokens = nullptr;
        unsigned sub_tokens_num = 0U;

        clang_tokenize(*context.TranslationUnit, cursor_extent, &sub_tokens, &sub_tokens_num);

        ParseAttributes(context, type, sub_tokens_num, sub_tokens);

        DISPOSE
        {
            clang_disposeTokens(*context.TranslationUnit, sub_tokens, sub_tokens_num);
        };
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

            HandleAttributes(context, cursor, clazz);

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
        auto field = visitor.GetScope().top()->asClazz()->Fields.emplace_back
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

        std::shared_ptr<MethodInfo> method = visitor.GetScope().top()->asClazz()->Methods.emplace_back(std::make_shared<MethodInfo>
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

        std::shared_ptr<TypeInfo> clazz = visitor.GetScope().top()->asType();
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