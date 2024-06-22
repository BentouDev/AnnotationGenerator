//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_ARGUMENTPARSER_H
#define ANNOTATIONGENERATOR_ARGUMENTPARSER_H

#include "Filesystem.h"
#include "Core/Context.h"

#include <nlohmann/json_fwd.hpp>
#include <vector>

class ArgumentParser
{
    Data::Context& Context;

    auto CreateTemplate(const fs::path& directory, const std::string& name) const -> std::unique_ptr<MustacheTemplate>;
    void ParseTemplates(const nlohmann::json& parser, const fs::path& directory, const std::vector<std::string>& files, const std::vector<std::string>& include_directories);
    void BuildContext(const fs::path& template_file, const std::vector<std::string>& files, const std::vector<std::string>& include_dirs);

public:
    bool Parse(int argc, const char** argv);

    explicit ArgumentParser(Data::Context& context);
};

#endif //ANNOTATIONGENERATOR_ARGUMENTPARSER_H
