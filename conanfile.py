from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import copy
import os

required_conan_version = ">=2.0"

class AgnesConan(ConanFile):
    name = "agnes"
    license = "MIT"
    url = "https://github.com/BentouDev/AnnotationGenerator"
    version = '0.6.0'

    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = ["Source/*", "**.yml"]

    build_requires = [
        "nlohmann_json/3.11.2",
        "kainjow_mustache/4.1@bentou/stable",
    ]

    tool_requires = ["fastbuild_installer/1.09@bentou/stable"] # "zetsubougen/[>=0.8.0]@bentou/stable", 
    python_requires = ["zetsubougen/[>=0.8.0]@bentou/stable"]
    python_requires_extend = "zetsubougen.ZetsubouBase"

    exports_sources = [
        "config/*",
        "Source/*",
        "build_tools.ini",
        "dependencies.ini",
        "project.yml",
    ]

    @property
    def project_file(self):
        return os.path.join(self.folders.base_source, 'project.yml')

    def export(self):
        copy(self, 'project.yml', self.recipe_folder, self.export_folder)
        copy(self, 'build_tools.ini', self.recipe_folder, self.export_folder)
        copy(self, 'dependencies.ini', self.recipe_folder, self.export_folder)

    # Binary package, host settings doesn't matter
    def package_id(self):
        del self.info.settings.compiler
        del self.info.settings.build_type
        self.info.python_requires.patch_mode()

    def validate(self):
        if self.info.settings.os not in ['Windows', 'Linux']:
            raise ConanInvalidConfiguration(f"This package is not compatible with '{self.info.settings.os}' yet!")

    def validate_build(self):
        if self.settings.compiler not in ['clang', 'msvc']:
            raise ConanInvalidConfiguration(f"This package cannot be built with '{self.settings.compiler}' yet!")

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
        self.folders.generators = "build/conan"
        self.cpp.build.bindirs = ["bin", "build/out"]

    def generate(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.configure()

    def build(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.build()

    def package(self):
        bin = os.path.join(self.package_folder, 'bin')
        if self.settings.os == "Windows":
            copy(self, "*/agnes.exe", dst=bin, src=self.build_folder, keep_path=False, ignore_case=True)
            copy(self, "*/libclang.dll", dst=bin, src=self.build_folder, keep_path=False, ignore_case=True)
        else:
            copy(self, "*/agnes", dst=bin, src=self.build_folder, keep_path=False, ignore_case=True)

    def package_info(self):
        for path in self.cpp.build.bindirs:
            bin = os.path.join(self.package_folder, path)
            self.buildenv_info.append_path('PATH', bin)

            if self.settings.os == "Windows":
                self.buildenv_info.append('AGNES_EXE', os.path.join(bin, "Agnes.exe"))
            if self.settings.os == "Linux":
                self.buildenv_info.append('AGNES_EXE', os.path.join(bin, "Agnes"))
