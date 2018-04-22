//
// Created by bentoo on 27.03.18.
//

#ifndef ANNOTATIONGENERATOR_RUNTIME_H
#define ANNOTATIONGENERATOR_RUNTIME_H

#include "Context.h"

class Runtime
{
    Data::Context& Context;

    void ProcessPattern (SourcePattern& pattern);
    void ParseSourceFile(SourceFile& file);
    void GenerateOutput (SourceFile& file);

public:
    explicit Runtime(Data::Context& context);

    int Run();
};


#endif //ANNOTATIONGENERATOR_RUNTIME_H
