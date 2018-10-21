//
// Created by bentoo on 10.09.17.
//

#include "ArgumentParser.h"
#include "Utils.h"
#include "../Core/Context.h"
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

ArgumentParser::ArgumentParser(Data::Context& context)
: Context(context)
{ }

bool ArgumentParser::Parse(int argc, char** argv)
{
    auto help_args = {
        "-h",
        "--help",
    };

    auto dir_args = {
        "-d",
        "--directory"
    };

    auto input_args = {
        "-i",
        "--input"
    };

    if (argc == 2 && Utils::MatchesAny(argv[1], help_args))
    {
        std::cout << "Clang based C++ preprocessor adding annotation based code generation" << std::endl;
        std::cout << "Usage: AnnotationGenerator [TEMPLATE] [FILES]..." << std::endl;
        std::cout << "       AnnotationGenerator [TEMPLATE] -i [INPUT_LIST_FILE]" << std::endl;
        std::cout << "\t [h]elp      - prints this info" << std::endl;
        std::cout << "\t [d]irectory - overrides output directory" << std::endl;
        return false;
    }

    if (argc <= 2)
    {
        std::cerr << "Too low number of arguments! Use --help for help." << std::endl;
        return false;
    }

    std::vector<std::string> parameters;
    fs::path                 input_list_file;
    for (int i = 1; i < argc; i++)
    {
        std::string param = argv[i];
        Utils::Trim(param);
        if (Utils::MatchesAny(param, dir_args))
        {
            if (i + 1 < argc)
            {
                i++;
                auto output = std::string(argv[i]);
                Context.Generator.OutputDirectory += output;
                std::cout << "Current output directory: " << output << std::endl;
            }
            else
            {
                std::cerr << "--directory parameter was used but no path was specified!" << std::endl;
                return false;
            }
        }
        else if (Utils::MatchesAny(param, input_args))
        {
            if (i + 1 < argc)
            {
                i++;
                auto input = std::string(argv[i]);
                input_list_file += input;
                std::cout << "Using input file: " << input << std::endl;
            }
            else
            {
                std::cerr << "--input parameter was used but no file was specified!" << std::endl;
                return false;
            }
        }
        else
        {
            parameters.emplace_back(argv[i]);
        }
    }

    std::cout << "Current working directory: " << fs::current_path() << std::endl;

    fs::path template_path;
    template_path += parameters[0];

    if (!input_list_file.empty())
    {
        std::ifstream if_handle(input_list_file, std::ios_base::in);
        if (if_handle)
        {
            for (std::string line; std::getline(if_handle, line);)
            {
                parameters.emplace_back(std::move(line));
            }
        }
    }

    parameters.erase(parameters.begin());

    BuildContext(template_path, parameters);

    return true;
}

std::unique_ptr<MustacheTemplate> ArgumentParser::CreateTemplate(const fs::path& directory, const std::string& name) const
{
    fs::path path = directory;
             path.append(name);

    return std::make_unique<MustacheTemplate>(path);
}

void ArgumentParser::ParseTemplates(const nlohmann::json& parser, const fs::path& directory, const std::vector<std::string>& files)
{
    for (auto& element : parser["patterns"])
    {
        auto& pattern = Context.Templates.emplace_back(std::make_unique<SourcePattern>());

        if (auto itr = element.find("includes"); itr != element.end())
        {
            pattern->UseIncludes = true;
        }

        if (auto inject_itr = element.find("inject_includes"); inject_itr != element.end())
        {
            for (auto& inject : (*inject_itr))
            {
                pattern->Includes.push_back(inject.get<std::string>());
            }
        }

        if (auto dir_itr = element.find("include_directories"); dir_itr != element.end())
        {
            for (auto& inject : (*dir_itr))
            {
                pattern->Directories.push_back(inject.get<std::string>());
            }
        }

        if (auto itr = element.find("require_annotation"); itr != element.end())
        {
            pattern->RequireAnnotation = true;
        }

        if (auto itr = element.find("annotation"); itr != element.end())
        {
            for (auto& annot : (*itr))
            {
                pattern->Annotations.push_back(annot.get<std::string>());
            }
        }

        if (Context.Generator.OutputDirectory.empty())
        {
            pattern->OutputDir = fs::current_path();

            if (auto itr = element.find("output_dir"); itr != element.end())
            {
                pattern->OutputDir.append(itr->get<std::string>());
            }
        }
        else
        {
            pattern->OutputDir = Context.Generator.OutputDirectory;
        }

        pattern->ClassOutName = element["class_out"] .get<std::string>();
        pattern->MainOutName  = element["main_out"]  .get<std::string>();
        pattern->MainTemplate = CreateTemplate(directory, element["main_template"].get<std::string>());

        for (auto& tmpl : element["class_template"])
        {
            pattern->Templates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
        }

        for (auto& file_path : files)
        {
            auto regex_string = element["file"].get<std::string>();
            const std::regex regex(regex_string);

            if (std::regex_match(file_path, regex))
            {
                pattern->Sources.emplace_back(std::make_unique<SourceFile>(file_path));
            }
        }
    }
}

void ArgumentParser::BuildContext(const fs::path& template_file, const std::vector<std::string>& files)
{
    using namespace nlohmann;

    if (!fs::exists(template_file))
    {
        std::cerr << "Unable to open template file " << template_file << "." << std::endl;
        return;
    }

    try
    {
        std::ifstream file(template_file);
        json          parser = json::parse(file);

        ParseTemplates(parser, template_file.parent_path(), files);
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to parse template file, cause :" << std::endl
                  << "\t" << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unable to parse template file due to uncaught exception." << std::endl;
    }
}
