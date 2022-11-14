from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.errors import ConanException
import os


class AgnesConan(ConanFile):
    name = "Agnes"
    license = "MIT"
    url = "https://github.com/BentouDev/AnnotationGenerator"
    version = '0.4.3'

    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = ["Source/*", "**.yml"]

    build_requires = [
        "nlohmann_json/3.11.2",
        "kainjow-mustache/4.1",
    ]

    tool_requires = ["zetsubougen/[>=0.6.4]@bentou/stable", "fastbuild-installer/1.07@bentou/stable"]
    python_requires = ["zetsubougen/[>=0.6.4]@bentou/stable"]
    python_requires_extend = "zetsubougen.ZetsubouBase"

    exports_sources = [
        "configurations/*",
        "platform/*",
        "Source/*",
        "build_tools.ini",
        "dependencies.ini",
        "**.yml",
    ]

    @property
    def project_file(self):
        return os.path.join(self.folders.base_source, 'project.yml')

    # Binary package, host settings doesn't matter
    def package_id(self):
        self.info.include_build_settings()
        del self.info.settings.compiler
        del self.info.settings.arch
        del self.info.settings.build_type

    def validate(self):
        if self.info.settings.os not in ['Windows', 'Linux']:
            raise ConanInvalidConfiguration(f"This package is not compatible with '{self.info.settings.os}' yet!")

    def validate_build(self):
        if self.settings.compiler not in ['Visual Studio', 'clang', 'msvc']:
            raise ConanInvalidConfiguration(f"This package cannot be built with '{self.settings.compiler}' yet!")

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
        self.folders.generators = "build/conan"
        self.cpp.build.bindirs = ["bin"]

    def generate(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.configure()

    def build(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.build()

    def package(self):
        if self.settings.os == "Windows":
            self.copy("*/Agnes.exe", dst="bin", src="", keep_path=False, ignore_case=True)
        else:
            self.copy("*/Agnes", dst="bin", src="", keep_path=False, ignore_case=True)

    def package_info(self):
        self.env_info.PATH.append(self.package_folder)

        if self.settings.os == "Windows":
            self.env_info.AGNES_EXE = os.path.join(self.package_folder, "Agnes.exe")
        if self.settings.os == "Linux":
            self.env_info.AGNES_EXE = os.path.join(self.package_folder, "Agnes")
