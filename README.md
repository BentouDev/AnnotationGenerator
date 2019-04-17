# AnnotationGenerator

Clang based C++ preprocessor for annotation based code generation

### Build status

|Windows|Linux|
|:-------:|:------:|
|[![Build status](https://ci.appveyor.com/api/projects/status/x82rh0rx426woaw0?svg=true)](https://ci.appveyor.com/project/MrJaqbq/annotationgenerator)|[![Build Status](https://travis-ci.org/BentouDev/AnnotationGenerator.svg?branch=master)](https://travis-ci.org/BentouDev/AnnotationGenerator)

### Usage

```bash
$ AnnotationGenerator [TEMPLATE] [FILES]...
$ AnnotationGenerator [TEMPLATE] -i [INPUT_LIST_FILE]
```

Where :

- FILES - list of files to parse,
- TEMPLATE - json configuration file,

``agnes.json``
```json
{
  "patterns" : [
    {
      "template" :[
        "template.cpp.mustache"
      ],
      "file" : "(\\W|\\w)*\\.h\\b",
      "annotation" : "Meta"
    }
  ]
}
```

``MyClass.h``
```cpp
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
class Meta(Serialize) MyClass
{
public:
    Meta(DoNotSerialize)
    int someField;
};
```

``template.cpp.mustache``
```django
// Generated file, do not modify!
#include "{{header}}.h"
template <> void registerReflection<{{class_name}}>()
{
    {{#fields}}
        registerField( {{name}}, &{{{canonical_name}}}::{{name}}, {{access}}, Meta::GetType<{{{type}}}> );
    {{/fields}}
};
```

### Dependencies

- libclang-5.0
- [nlohmann/json](https://github.com/nlohmann/json) (as conan package)
- [kainjow/Mustache](https://github.com/kainjow/Mustache) (as conan package)
