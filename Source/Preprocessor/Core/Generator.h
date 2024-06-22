//
// Created by bentoo on 27.03.18.
//

#ifndef ANNOTATIONGENERATOR_GENERATOR_H
#define ANNOTATIONGENERATOR_GENERATOR_H

#include "Context.h"

class Generator
{
    using TTypeCache = std::map<std::string, TMustacheData>;

    Data::Context& Context;
    TTypeCache     Cache;

    std::string   BuildIncludePath     (std::shared_ptr<TypeInfo> type);

    TMustacheData BuildIncludesData    ();
    TMustacheData BuildAllClassData    ();
    TMustacheData BuildAllEnumData     ();
    TMustacheData BuildAttributeData   (std::shared_ptr<TypeInfo> type);
    TMustacheData BuildEnumData        (std::shared_ptr<EnumInfo>& type);
    TMustacheData BuildEnumValuesData  (std::shared_ptr<EnumInfo>& type);
    TMustacheData BuildClazzData       (std::shared_ptr<ClassInfo>& type);
    TMustacheData BuildFieldData       (std::shared_ptr<ClassInfo>& type);
    TMustacheData BuildMethodData      (std::shared_ptr<ClassInfo>& type);
    TMustacheData BuildBaseClassesData (std::shared_ptr<ClassInfo>& type);

    fs::path      BuildHeaderOutputPath(const std::string& include_name);
    fs::path      BuildUnitOutputPath(const std::string& include);
    fs::path      BuildClassOutputPath (std::shared_ptr<ClassInfo> type);
    fs::path      BuildEnumOutputPath  (std::shared_ptr<EnumInfo> type);

public:
    explicit Generator(Data::Context& context);

    void GenerateFiles();
};


#endif //ANNOTATIONGENERATOR_GENERATOR_H
