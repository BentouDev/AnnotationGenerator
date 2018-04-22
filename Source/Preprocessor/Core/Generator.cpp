//
// Created by bentoo on 27.03.18.
//

#include <fstream>
#include "Generator.h"
#include "Context.h"

Generator::Generator(Data::Context& context)
    : Context(context)
{

}

TMustacheData Generator::BuildTypeData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData data;
    data.set("class_name", type->Name);
    data.set("fields", BuildFieldData (type));
    data.set("methods", BuildMethodData(type));

    return data;
}

TMustacheData Generator::BuildFieldData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData fields {TMustacheData::type::list};

    for (auto& field : type->Fields)
    {
        TMustacheData field_data;
        field_data.set("name", field->Name);
        field_data.set("type", field->BaseType.Name);
        field_data.set("access", field->AccessType);
        field_data.set("offset", "0");

        fields << field_data;
    }

    return fields;
}

TMustacheData Generator::BuildMethodData(std::shared_ptr<ClassInfo>& type)
{
    TMustacheData methods{TMustacheData::type::list};

    for (auto& method : type->Methods)
    {
        TMustacheData method_data;
        method_data.set("name", method->Name);
        method_data.set("return_type", method->ReturnType.Name);
        method_data.set("offset", "0");

        methods << method_data;
    }

    return methods;
}

fs::path Generator::BuildOutputPath(std::unique_ptr<MustacheTemplate>& templ, std::shared_ptr<ClassInfo>& type)
{
    fs::path    path      = templ->Path.parent_path();
    std::string filename  = "gen_";
    filename += type->Name;
    filename += ".h";

    path.append(filename);

    return path;
}

void Generator::GenerateFiles()
{
    for (auto& templ : Context.Generator.CurrentPattern->Templates)
    {
        for (auto& type : Context.Parser.Classes)
        {
            fs::path     path = BuildOutputPath(templ, type);
            std::fstream file(path, std::ios_base::out);

            file << templ->View->render(BuildTypeData(type));
            file.close();
        }
    }
}
