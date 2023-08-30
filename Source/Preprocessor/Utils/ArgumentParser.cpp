//
// Created by bentoo on 10.09.17.
//

#include "ArgumentParser.h"
#include "Utils.h"
#include "Core/Context.h"
#include "Version.h"

#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <format>

ArgumentParser::ArgumentParser(Data::Context& context)
: Context(context)
{ }

bool ArgumentParser::Parse(int argc, char** argv)
{
    auto help_args = {
        "-h",
        "--help",
    };

    auto version_args = {
        "-v",
        "--version",
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

    if (argc == 2 && Utils::MatchesAny(argv[1], version_args))
    {
        const auto now = std::chrono::system_clock::now();
        std::cout << "Agnes " << AGNES_VERSION << " - Copyright BentouDev @ 2017-" << std::format("{:%Y}", now) << std::endl;
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

        // will include source header into worker file
        // instead of copying it's lines into it
        if (auto itr = element.find("include_source_header"); itr != element.end())
        {
            pattern->IncludeSourceHeader = true;
        }

        // includes to be injected into worker file
        // injected includes are also parsed by libclang
        if (auto inject_itr = element.find("inject_includes"); inject_itr != element.end())
        {
            for (auto& inject : (*inject_itr))
            {
                pattern->InjectIncludes.push_back(inject.get<std::string>());
            }
        }

        // directories to search injected includes in
        if (auto dir_itr = element.find("include_directories"); dir_itr != element.end())
        {
            for (auto& inject : (*dir_itr))
            {
                pattern->IncludeDirectories.push_back(inject.get<std::string>());
            }
        }

        if (auto itr = element.find("require_annotation"); itr != element.end() && itr->is_boolean())
        {
            pattern->RequireAnnotation = itr->get<bool>();
        }

        // variables of this type placed before class definition or field
        // are treaded as annotations
        // requires specialized header with macro which enables these only for ANNOTATION_GENERATOR
        // so they are not placed in normal code
        // variables of this type are also ignored from field lists
        if (auto itr = element.find("enable_pre_annotation"); itr != element.end())
        {
            for (auto& annot : (*itr))
            {
                pattern->EnablePreAnnotationFrom.push_back(annot.get<std::string>());
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

        pattern->MainOutName = element["main_file_name"].get<std::string>();
        pattern->MainTemplate = CreateTemplate(directory, element["main_template"].get<std::string>());

        pattern->HeaderOutName = element["header_file_name"].get<std::string>();
        for (auto& tmpl : element["header_template"])
        {
            pattern->HeaderTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
        }

        pattern->ClassOutName = element["class_file_name"].get<std::string>();
        for (auto& tmpl : element["class_template"])
        {
            pattern->ClassTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
        }

        pattern->EnumOutName = element["enum_file_name"].get<std::string>();
        for (auto& tmpl : element["enum_template"])
        {
            pattern->EnumTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
        }

        for (auto& file_path : files)
        {
            auto regex_string = element["file_regex_pattern"].get<std::string>();
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
