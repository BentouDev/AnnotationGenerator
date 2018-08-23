//
// Created by bentoo on 27.03.18.
//

#include <fstream>
#include "Generator.h"
#include "Context.h"
#include "../Utils/Utils.h"

Generator::Generator(Data::Context& context)
    : Context(context)
{

}

TMustacheData Generator::BuildAllData()
{
    TMustacheData types { TMustacheData::type::list };

    // std::string& ann_name = Context.Generator.CurrentPattern->Annotation;
    for (auto& [_, type] : Context.Parser.Classes)
    {
        UNUSED(_);

        if (type->HasAnnotation || !Context.Generator.CurrentPattern->RequireAnnotation)
        {
            TMustacheData data = BuildTypeData(type);

            Cache.insert(std::make_pair(type->CanonName, data));

            types << data;
        }
    }

    return types;
}

TMustacheData Generator::BuildTypeData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData data;
    data.set("fields",     BuildFieldData (type));
    data.set("methods",    BuildMethodData(type));
    data.set("attributes", BuildAttributeData(type));
    data.set("class_name", type->Name);
    data.set("canonical_name", type->CanonName);
    data.set("include",    type->FromInclude);

    return data;
}

TMustacheData Generator::BuildFieldData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData fields { TMustacheData::type::list };

    for (auto& field : type->Fields)
    {
        TMustacheData field_data;
        field_data.set("name", field->Name);
        field_data.set("type", field->BaseType->Name);
        field_data.set("access", field->AccessType);

        fields << field_data;
    }

    return fields;
}

TMustacheData Generator::BuildMethodData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData methods { TMustacheData::type::list };

    for (auto& method : type->Methods)
    {
        TMustacheData method_data;
        method_data.set("name", method->Name);
        method_data.set("return_type", method->ReturnType->Name);

        methods << method_data;
    }

    return methods;
}

TMustacheData Generator::BuildAttributeData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData attributes { TMustacheData::type::list };

    for (auto& annotation : type->Annotations)
    {
        TMustacheData attribute_data;
        attribute_data.set("value", annotation);

        attributes << attribute_data;
    }

    return attributes;
}

fs::path Generator::BuildOutputPath(std::shared_ptr<ClassInfo>& type)
{
    TMustacheView out_name_view(Context.Generator.CurrentPattern->ClassOutName);

    fs::path    path      = Context.Generator.CurrentPattern->OutputDir;
    std::string filename  = out_name_view.render({"class_name", type->Name});
    path.append(filename);

    return path;
}

void Generator::GenerateFiles()
{
    TMustacheData all_data;
    all_data.set("classes", BuildAllData());

    fs::create_directories(Context.Generator.CurrentPattern->OutputDir);

    for (auto& templ : Context.Generator.CurrentPattern->Templates)
    {
        if (templ->View)
        {
            for (auto& [_, type] : Context.Parser.Classes)
            {
                UNUSED(_);

                if (auto itr = Cache.find(type->CanonName); itr != Cache.end())
                {
                    fs::path     path = BuildOutputPath(type);
                    std::fstream file(path, std::ios_base::out);

                    file << templ->View->render(itr->second);
                    file.close();
                }
            }
        }
    }

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
