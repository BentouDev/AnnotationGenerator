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
    Context& CurrentContext;

    void ParseTemplates(const nlohmann::json& parser, const std::vector<std::string>& files);
    void BuildContext(const fs::path& template_file, const std::vector<std::string>& files);

public:
    bool Parse(int argc, char* argv[]);

    explicit ArgumentParser(Context &options);
};

#endif //ANNOTATIONGENERATOR_ARGUMENTPARSER_H
