//
// Created by bentoo on 10.09.17.
//

#ifndef ANNOTATIONGENERATOR_ARGUMENTPARSER_H
#define ANNOTATIONGENERATOR_ARGUMENTPARSER_H

#include <vector>
#include <nlohmann/json_fwd.hpp>
#include "Argument.h"
#include "Filesystem.h"
#include "../Core/Context.h"

class ArgumentParser
{
    Data::Context& Context;

    auto CreateTemplate(const fs::path& directory, const std::string& name) const -> std::unique_ptr<MustacheTemplate>;
    void ParseTemplates(const nlohmann::json& parser, const fs::path& directory, const std::vector<std::string>& files);
    void BuildContext(const fs::path& template_file, const std::vector<std::string>& files);

public:
    bool Parse(int argc, char* argv[]);

    explicit ArgumentParser(Data::Context& context);
};

#endif //ANNOTATIONGENERATOR_ARGUMENTPARSER_H
