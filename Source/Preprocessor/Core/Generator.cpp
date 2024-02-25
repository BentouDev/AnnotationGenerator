//
// Created by bentoo on 27.03.18.
//

#include "Generator.h"
#include "Context.h"
#include "Utils/Utils.h"

#include <fstream>

Generator::Generator(Data::Context& context)
    : Context(context)
{

}

TMustacheData Generator::BuildAllClassData()
{
    TMustacheData types { TMustacheData::type::list };

    // std::string& ann_name = Context.Generator.CurrentPattern->Annotation;
    for (auto& [_, type] : Context.Parser.Classes)
    {
        UNUSED(_);

        if (Context.Generator.CurrentPattern->RequireAnnotation.empty() || type->HasAnnotation(Context.Generator.CurrentPattern->RequireAnnotation))
        {
            TMustacheData data = BuildClazzData(type);

            Cache.insert(std::make_pair(type->CanonName, data));

            types << data;
        }
    }

    return types;
}

TMustacheData Generator::BuildAllEnumData()
{
    TMustacheData types{ TMustacheData::type::list };

    for (auto& [_, type] : Context.Parser.Enums)
    {
        UNUSED(_);

        if (Context.Generator.CurrentPattern->RequireAnnotation.empty() || type->HasAnnotation(Context.Generator.CurrentPattern->RequireAnnotation))
        {
            TMustacheData data = BuildEnumData(type);

            Cache.insert(std::make_pair(type->CanonName, data));

            types << data;
        }
    }

    return types;
}

std::string Generator::BuildIncludePath(std::shared_ptr<TypeInfo> type)
{
    fs::path absolute;
    absolute.append(type->FromInclude);

    std::string include_path = absolute.string();

    std::vector<std::string> relatives{ fs::relative(absolute).string() };

    for (auto& incl_dir : Context.Generator.CurrentPattern->IncludeDirectories)
    {
        fs::path dir;
        dir.append(incl_dir);

        std::error_code err;
        relatives.push_back(fs::relative(absolute, dir, err).string());
    }

    relatives.erase(std::remove_if(relatives.begin(), relatives.end(), [](const std::string& str) {
        return str.empty();
    }), relatives.end());

    auto itr = std::min_element(relatives.begin(), relatives.end(),
        [](const std::string& s1, const std::string& s2) {
            return s1.length() < s2.length();
        }
    );

    if (itr != relatives.end())
    {
        include_path = *itr;
    }

    return include_path;
}

TMustacheData Generator::BuildEnumData(std::shared_ptr<EnumInfo>& type)
{
    TMustacheData data;

    data.set("attributes", BuildAttributeData(type));
    data.set("enum_name", type->Name);
    data.set("canonical_name", type->CanonName);
    data.set("values", BuildEnumValuesData(type));
    data.set("include", BuildIncludePath(type));

    return data;
}

TMustacheData Generator::BuildEnumValuesData(std::shared_ptr<EnumInfo>& type)
{
    TMustacheData values{ TMustacheData::type::list };

    for (const auto& enum_value : type->Values)
    {
        TMustacheData value_data;

        value_data.set("name", enum_value->Name);
        value_data.set("value", std::to_string(enum_value->NumValue));

        values << value_data;
    }

    return values;
}

TMustacheData Generator::BuildClazzData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData data;

    data.set("fields",     BuildFieldData (type));
    data.set("methods",    BuildMethodData(type));
    data.set("attributes", BuildAttributeData(type));
    data.set("class_name", type->Name);
    data.set("canonical_name", type->CanonName);
    data.set("include",   BuildIncludePath(type));

    return data;
}

TMustacheData Generator::BuildFieldData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData fields { TMustacheData::type::list };

    for (const auto& field : type->Fields)
    {
        TMustacheData field_data;
        field_data.set("name", field->Name);
        field_data.set("type", field->BaseType->Name);
        field_data.set("access", std::to_string(field->AccessType));

        fields << field_data;
    }

    return fields;
}

TMustacheData Generator::BuildMethodData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData methods { TMustacheData::type::list };

    for (const auto& method : type->Methods)
    {
        TMustacheData method_data;
        method_data.set("name", method->Name);
        method_data.set("return_type", method->ReturnType->Name);

        methods << method_data;
    }

    return methods;
}

TMustacheData Generator::BuildAttributeData(std::shared_ptr<TypeInfo> type)
{
    TMustacheData attributes { TMustacheData::type::list };

    for (const auto& annotation : type->Annotations)
    {
        TMustacheData attribute_data;
        attribute_data.set("value", annotation);

        attributes << attribute_data;
    }

    return attributes;
}

fs::path Generator::BuildHeaderOutputPath(const std::string& include)
{
    TMustacheView out_name_view(Context.Generator.CurrentPattern->HeaderOutName);

    fs::path path = Context.Generator.CurrentPattern->OutputDir;
    std::string filename = out_name_view.render({ "header_name", include });
    path.append(filename);

    return path;
}

fs::path Generator::BuildUnitOutputPath(const std::string& include)
{
    TMustacheView out_name_view(Context.Generator.CurrentPattern->UnitOutName);

    fs::path path = Context.Generator.CurrentPattern->OutputDir;
    std::string filename = out_name_view.render({ "header_name", include });
    path.append(filename);

    return path;
}

fs::path Generator::BuildClassOutputPath(std::shared_ptr<ClassInfo> type)
{
    TMustacheView out_name_view(Context.Generator.CurrentPattern->ClassOutName);

    fs::path path = Context.Generator.CurrentPattern->OutputDir;
    std::string filename = out_name_view.render({ "class_name", type->Name });
    path.append(filename);

    return path;
}

fs::path Generator::BuildEnumOutputPath(std::shared_ptr<EnumInfo> type)
{
    TMustacheView out_name_view(Context.Generator.CurrentPattern->EnumOutName);

    fs::path    path = Context.Generator.CurrentPattern->OutputDir;
    std::string filename = out_name_view.render({ "enum_name", type->Name });
    path.append(filename);

    return path;
}

TMustacheData Generator::BuildIncludesData()
{
    TMustacheData data { TMustacheData::type::list };

    for (std::unique_ptr<SourceFile>& src : Context.Parser.CurrentPattern->Sources)
    {
        TMustacheData inc_data;
        inc_data.set("path", src->Path.string());
        data << inc_data;
    }

    return data;
}

static TMustacheData BuildGenIncludesData(const std::vector<std::string>& includes)
{
    TMustacheData data{ TMustacheData::type::list };

    for (const std::string& str : includes)
    {
        TMustacheData inc_data;
        inc_data.set("path", str);
        data << inc_data;
    }

    return data;
}

void Generator::GenerateFiles()
{
    TMustacheData all_data;
    all_data.set("classes", BuildAllClassData());
    all_data.set("enums", BuildAllEnumData());
    all_data.set("includes", BuildIncludesData());

    fs::create_directories(Context.Generator.CurrentPattern->OutputDir);

    // Per header template
    struct IncludeData
    {
        std::vector<std::shared_ptr<ClassInfo>> classes;
        std::vector<std::shared_ptr<EnumInfo>> enums;
    };

    std::map<std::string, IncludeData> include_map;

    for (auto& [_, type] : Context.Parser.Classes)
    {
        include_map[type->FromInclude].classes.push_back(type);
    }

    for (auto& [_, type] : Context.Parser.Enums)
    {
        include_map[type->FromInclude].enums.push_back(type);
    }

    auto emit_per_file = [&]<typename TFunc>
    (
        std::vector<std::string>* out_gen_paths,
        const std::unique_ptr<MustacheTemplate>& templ, TFunc&& func
    )
    {
        if (templ->View)
        {
            for (auto& [include, data] : include_map)
            {
                TMustacheData mapped_data;

                TMustacheData classes{ TMustacheData::type::list };
                for (auto& clazz : data.classes)
                {
                    if (auto itr = Cache.find(clazz->CanonName); itr != Cache.end())
                    {
                        classes << itr->second;
                    }
                }

                TMustacheData enums{ TMustacheData::type::list };
                for (auto& clazz : data.enums)
                {
                    if (auto itr = Cache.find(clazz->CanonName); itr != Cache.end())
                    {
                        enums << itr->second;
                    }
                }

                mapped_data.set("header_path", include);
                mapped_data.set("classes", classes);
                mapped_data.set("enums", enums);

                fs::path inc_path = include;

                fs::path path = func(inc_path.stem().string());

                if (out_gen_paths)
                {
                    out_gen_paths->push_back(path.string());
                }

                std::fstream file(path, std::ios_base::out);

                file << templ->View->render(mapped_data);
                file.close();
            }
        }
    };

    std::vector<std::string> gen_includes;

    for (auto& templ : Context.Generator.CurrentPattern->HeaderTemplates)
    {
        emit_per_file(&gen_includes, templ, [&](const std::string& include) { return BuildHeaderOutputPath(include); });
    }

    for (auto& templ : Context.Generator.CurrentPattern->UnitTemplates)
    {
        emit_per_file(nullptr, templ, [&](const std::string& include) { return BuildUnitOutputPath(include); });
    }

    all_data.set("gen_includes", BuildGenIncludesData(gen_includes));

    // Class templates
    for (auto& templ : Context.Generator.CurrentPattern->ClassTemplates)
    {
        if (templ->View)
        {
            for (auto& [_, type] : Context.Parser.Classes)
            {
                UNUSED(_);

                if (auto itr = Cache.find(type->CanonName); itr != Cache.end())
                {
                    fs::path     path = BuildClassOutputPath(type);
                    std::fstream file(path, std::ios_base::out);

                    file << templ->View->render(itr->second);
                    file.close();
                }
            }
        }
    }

    // Enum templates
    for (auto& templ : Context.Generator.CurrentPattern->EnumTemplates)
    {
        if (templ->View)
        {
            for (auto& [_, type] : Context.Parser.Enums)
            {
                UNUSED(_);

                if (auto itr = Cache.find(type->CanonName); itr != Cache.end())
                {
                    fs::path     path = BuildEnumOutputPath(type);
                    std::fstream file(path, std::ios_base::out);

                    file << templ->View->render(itr->second);
                    file.close();
                }
            }
        }
    }

    // Main template
    auto& main_tmpl = Context.Generator.CurrentPattern->MainTemplate;
    if (main_tmpl->View)
    {
        fs::path main_path = Context.Generator.CurrentPattern->OutputDir;
        main_path.append(Context.Generator.CurrentPattern->MainOutName);

        std::fstream file(main_path, std::ios_base::out);
        file << main_tmpl->View->render(all_data);
        file.close();
    }
}
