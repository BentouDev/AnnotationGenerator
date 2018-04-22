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

void Generator::GenerateFiles()
{
    for (auto& templ : Context.Generator.CurrentPattern->Templates)
    {
        for (auto& type : Context.Parser.Classes)
        {
            TMustacheData data;
            data.set("class_name", type->Name);

            TMustacheData fields {TMustacheData::type::list};
            TMustacheData methods{TMustacheData::type::list};

            for (auto& field : type->Fields)
            {
                TMustacheData field_data;
                field_data.set("name", field->Name);
                field_data.set("type", field->BaseType.Name);
                field_data.set("access", field->AccessType);
                field_data.set("offset", "0");

                fields << field_data;
            }

            data.set("fields", fields);

            for (auto& method : type->Methods)
            {
                TMustacheData method_data;
                method_data.set("name", method->Name);
                method_data.set("return_type", method->ReturnType.Name);
                method_data.set("offset", "0");

                methods << method_data;
            }

            data.set("methods", methods);

            fs::path    path      = templ->Path.parent_path();
            std::string filename  = "gen_";
                        filename += type->Name;
                        filename += ".h";

            path.append(filename);

            std::fstream file(path, std::ios_base::out);
            file << templ->View->render(data);
            file.close();
        }
    }
}
