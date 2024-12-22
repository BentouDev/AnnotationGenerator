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
#include <functional>

class IParamReader
{
public:
    virtual ~IParamReader() = default;

    virtual std::string_view next() = 0;
    virtual bool has_next() = 0;
};

class ArgParamReader : public IParamReader
{
public:
    ArgParamReader(int argc, const char** argv)
        : args(argv, size_t(argc))
    { }

    std::string_view next() override
    {
        return args[idx++];
    }

    bool has_next() override
    {
        return idx < args.size();
    }

    std::span<const char*> args;
    unsigned idx = 0;
};

class FileParamReader : public IParamReader
{
public:
    FileParamReader(std::ifstream& stream)
        : file(stream)
    { }

    std::string_view next() override
    {
        if (buffer_idx >= buffer.size())
        {
            // fill buffer
            if (std::getline(file, line))
            {
                Utils::SplitBySpace(line, buffer);
                buffer_idx = 0;
            }
        }

        if (buffer_idx >= buffer.size())
        {
            throw new std::exception("Code error! Read past file buffer!");
        }

        return buffer[buffer_idx];
    }

    bool has_next() override
    {
        return buffer_idx < buffer.size() || !file.eof();
    }

    std::ifstream& file;
    std::string line;
    std::vector<std::string_view> buffer;
    unsigned buffer_idx = 0;
};

ArgumentParser::ArgumentParser(Data::Context& context)
: Context(context)
{ }

bool ArgumentParser::Parse(int argc, const char** argv)
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

    auto list_headers = {
        "-f",
        "--files"
    };

    auto list_include_dirs = {
        "-i",
        "--include_dirs"
    };

    enum class EArgumentListMode
    {
        Headers,
        IncludeDirectories
    };

    EArgumentListMode mode = EArgumentListMode::Headers;

    if (argc == 2 && Utils::MatchesAny(argv[1], help_args))
    {
        std::cout << "Clang based C++ preprocessor adding annotation based code generation" << std::endl;
        std::cout << "Usage: AnnotationGenerator [TEMPLATE] [FILES]..." << std::endl;
        std::cout << "       AnnotationGenerator @[RESPONSE_FILE]" << std::endl;
        std::cout << std::endl;
        std::cout << "\t [f]iles         - (default) following arguments treated as input headers" << std::endl;
        std::cout << "\t [i]include_dirs - following arguments treated as include directories" << std::endl;
        std::cout << "\t [h]elp          - prints this info" << std::endl;
        std::cout << "\t [d]irectory     - overrides output directory" << std::endl;
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

    std::optional<std::string> template_file;
    std::vector<std::string> headers;
    std::vector<std::string> include_dirs;

    auto parse_argument = [&](IParamReader& reader) -> bool {
        std::string_view param = reader.next();
        if (!template_file.has_value())
        {
            template_file = param;
            return true;
        }

        if (Utils::MatchesAny(param, dir_args))
        {
            if (reader.has_next())
            {
                std::string_view output = reader.next();
                Context.Generator.OutputDirectory += output;
                std::cout << "Current output directory: " << output << std::endl;
            }
            else
            {
                std::cerr << "--directory parameter was used but no path was specified!" << std::endl;
                return false;
            }
        }
        else if (Utils::MatchesAny(param, list_headers))
        {
            if (reader.has_next())
            {
                mode = EArgumentListMode::Headers;
            }
            else
            {
                std::cerr << "--files parameter was used but no files were provided!" << std::endl;
                return false;
            }
        }
        else if (Utils::MatchesAny(param, list_include_dirs))
        {
            if (reader.has_next())
            {
                mode = EArgumentListMode::IncludeDirectories;
            }
            else
            {
                std::cerr << "--include_dirs parameter was used but no dirs were provided!" << std::endl;
                return false;
            }
        }
        else
        {
            switch (mode)
            {
                case EArgumentListMode::Headers:
                {
                    headers.emplace_back(param);
                    break;
                }
                case EArgumentListMode::IncludeDirectories:
                {
                    include_dirs.emplace_back(param);
                    break;
                }
            }
        }

        return true;
    };

    
    std::string_view assumed_response_file = std::string_view{ argv[2] };
    if (assumed_response_file.starts_with("@"))
    {
        // Response file
        fs::path input_list_file = assumed_response_file.substr(1);
        std::ifstream if_handle(input_list_file, std::ios_base::in);
        if (if_handle)
        {
            FileParamReader file_reader{ if_handle };
            while (file_reader.has_next())
            {
                if (!parse_argument(file_reader))
                {
                    return false;
                }
            }

            //for (std::string line; std::getline(if_handle, line);)
            //{
            //    std::vector<std::string_view> fragments;
            //    Utils::SplitBySpace(line, fragments);

            //    int fragment_idx = 0;
            //    auto get_arg = [&]() {
            //        return fragments[fragment_idx++];
            //    };

            //    auto has_next = []() {
            //        return fragment_idx
            //    };

            //    if (!parse_argument(line, !if_handle.eof()))
            //    {
            //        return false;
            //    }
            //}
        }
        else
        {
            std::cerr << "Unable to open response file '" << input_list_file.string() << "'!" << std::endl;
            return false;
        }

        std::cerr << "Response files are not yet implemented!" << std::endl;
        return false;
    }
    else
    {
        // Commandline args
        ArgParamReader arg_reader{ argc - 1, argv + 1 };

        while (arg_reader.has_next())
        {
            if (!parse_argument(arg_reader))
            {
                return false;
            }
        }

        //for (int i = 1; i < argc; i++)
        //{
        //    std::string param = argv[i];
        //    Utils::Trim(param);

        //    if (!parse_argument([&]() -> std::string_view { return param; }, [&]() -> bool { return i + 1 < argc; }))
        //    {
        //        return false;
        //    }
        //}
    }

    std::cout << "Current working directory: " << fs::current_path() << std::endl;

    fs::path template_path = template_file.value();

    if (!BuildContext(template_path, headers, include_dirs))
    {
        return false;
    }

    return true;
}

std::unique_ptr<MustacheTemplate> ArgumentParser::CreateTemplate(const fs::path& directory, const std::string& name) const
{
    fs::path path = directory;
             path.append(name);

    return std::make_unique<MustacheTemplate>(path);
}

void ArgumentParser::ParseTemplates(const nlohmann::json& parser, const fs::path& directory, const std::vector<std::string>& files, const std::vector<std::string>& include_directories)
{
    for (auto& element : parser["patterns"])
    {
        auto try_get_str = [&](const char* str)
        {
            auto itr = element.find(str);
            if (itr != element.end())
            {
                return (*itr).get<std::string>();
            }

            return std::string{};
        };

        auto& pattern = Context.Templates.emplace_back(std::make_unique<SourcePattern>());

        // will include source header into worker file
        // instead of copying it's lines into it
        if (auto itr = element.find("include_source_header"); itr != element.end())
        {
            pattern->IncludeSourceHeader = itr.value().get<bool>();
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

        for (const std::string& inc_dir : include_directories)
        {
            pattern->IncludeDirectories.push_back(inc_dir);
        }

        if (auto itr = element.find("require_annotation"); itr != element.end())
        {
            for (auto& annot : (*itr))
            {
                pattern->RequireAnnotation.push_back(annot.get<std::string>());
            }
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

        pattern->MainOutName = try_get_str("main_file_name");
        pattern->MainTemplate = CreateTemplate(directory, try_get_str("main_template"));

        pattern->HeaderOutName = try_get_str("header_file_name");
        if (auto itr = element.find("header_template"); itr != element.end())
        {
            for (auto& tmpl : itr.value())
            {
                pattern->HeaderTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
            }
        }

        pattern->UnitOutName = try_get_str("unit_file_name");
        if (auto itr = element.find("unit_template"); itr != element.end())
        {
            for (auto& tmpl : itr.value())
            {
                pattern->UnitTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
            }
        }

        pattern->ClassOutName = try_get_str("class_file_name");
        if (auto itr = element.find("class_template"); itr != element.end())
        {
            for (auto& tmpl : itr.value())

            {
                pattern->ClassTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
            }
        }

        pattern->EnumOutName = try_get_str("enum_file_name");

        if (auto itr = element.find("enum_template"); itr != element.end())
        {
            for (auto& tmpl : itr.value())
            {
                pattern->EnumTemplates.emplace_back(CreateTemplate(directory, tmpl.get<std::string>()));
            }
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

bool ArgumentParser::BuildContext(const fs::path& template_file, const std::vector<std::string>& files, const std::vector<std::string>& include_dirs)
{
    using namespace nlohmann;

    if (!fs::exists(template_file))
    {
        std::cerr << "Unable to open template file " << template_file << "." << std::endl;
        return false;
    }

    try
    {
        std::ifstream file(template_file);
        json          parser = json::parse(file,
            nullptr, // callback
            true, // allow exceptions
            true); // ignore comments

        ParseTemplates(parser, template_file.parent_path(), files, include_dirs);
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to parse template file " << fs::absolute(template_file) << ", cause :" << std::endl
                  << "\t" << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Unable to parse template file " << fs::absolute(template_file) << " due to uncaught exception." << std::endl;
        return false;
    }

    return true;
}
