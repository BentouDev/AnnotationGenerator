# AnnotationGenerator

Clang based C++ preprocessor for annotation based reflection generation

### Build status

|Windows|Linux|Coverage
|:-------:|:------:|:------:|
|[![Build status](https://ci.appveyor.com/api/projects/status/x82rh0rx426woaw0?svg=true)](https://ci.appveyor.com/project/MrJaqbq/annotationgenerator)|[![Build Status](https://travis-ci.org/BentouDev/AnnotationGenerator.svg?branch=master)](https://travis-ci.org/BentouDev/AnnotationGenerator)| ;^)

### Usage

```bash
$ AnnotationGenerator [TEMPLATE] [FILES]...
```

Where :

- FILES - list of files to parse,
- TEMPLATE - json configuration file,

```json
{
  "patterns" : [
    {
      "template" :[
        "output_file_template.mustache"
      ],
      "file" : "(\\W|\\w)*\\.h\\b",
      "annotation" : "Meta"
    }
  ]
}
```

### Dependencies

- libclang-5.0
- [nlohmann/json](https://github.com/nlohmann/json) (provided as a submodule)
- [kainjow/Mustache](https://github.com/kainjow/Mustache) (provided as a submodule)
