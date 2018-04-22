//
// Created by bentoo on 10.09.17.
//

#include "ArgumentParser.h"
#include "Utils.h"
#include <regex>
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

    if (argc == 2 && Utils::MatchesAny(argv[1], help_args))
    {
        std::cout << "Clang based C++ preprocessor adding annotation based code generation" << std::endl;
        std::cout << "Usage: AnnotationGenerator [TEMPLATE] [FILES]..." << std::endl;
        std::cout << "\t help  - prints this info" << std::endl;
        return false;
    }

    if (argc <= 2)
    {
        std::cerr << "Too low number of arguments! Use --help for help." << std::endl;
        return false;
    }

    std::cout << "Current directory: " << fs::current_path() << std::endl;

    fs::path template_path;
    template_path += argv[1];

    std::vector<std::string> template_files;
    for (int i = 2; i < argc; i++)
    {
        template_files.emplace_back(argv[i]);
    }

    BuildContext(template_path, template_files);

    return true;
}

void ArgumentParser::ParseTemplates(const nlohmann::json& parser, const fs::path& directory, const std::vector<std::string>& files)
{
    for (auto& element : parser["patterns"])
    {
        auto& pattern = Context.Templates.emplace_back(std::make_unique<SourcePattern>());

        for (auto& tmpl : element["template"])
        {
            fs::path path = directory;
                     path.append(tmpl.get<std::string>());

            pattern->Templates.emplace_back(std::make_unique<MustacheTemplate>(path));
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
