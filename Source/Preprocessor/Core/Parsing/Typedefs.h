//
// Created by bentoo on 22.04.18.
//

#ifndef ANNOTATIONGENERATOR_TYPEDEFS_H
#define ANNOTATIONGENERATOR_TYPEDEFS_H

#include <ReflectionInfo.h>

#include <clang-c/Index.h>
#include <functional>
#include <memory>

class ParseContext;
class Visitor;

using TCursorResolveResult = std::pair<bool, std::shared_ptr<MetaInfo>>;
using TCursorTypeHandler   = std::function<TCursorResolveResult(ParseContext&, CXCursor, Visitor&)>;

#endif //ANNOTATIONGENERATOR_TYPEDEFS_H
