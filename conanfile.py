from conans import ConanFile
import os


class AgnesConan(ConanFile):
    name = "Agnes"
    license = "MIT"
    url = "https://github.com/BentouDev/AnnotationGenerator"

    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = ["Source/*", "**.yml"]

    build_requires = [
        "jsonformoderncpp/3.6.1@vthiery/stable",
        "kainjow-mustache/3.2.1@bincrafters/stable",
    ]

    tool_requires = ["zetsubougen/[>=0.6.4]@bentou/stable", "fastbuild-installer/1.07@bentou/stable"]
    python_requires = ["zetsubougen/[>=0.6.4]@bentou/stable"]
    python_requires_extend = "zetsubougen.ZetsubouBase"

    @property
    def project_file(self):
        return os.path.join(self.folders.base_source, 'project.yml')

    # Binary package, host settings doesn't matter
    def package_id(self):
        self.info.include_build_settings()
        del self.info.settings.compiler
        del self.info.settings.arch
        del self.info.settings.build_type

    def generate(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.configure()

    def build(self):
        self.zetsubou.init(self, project_file=self.project_file)
        self.zetsubou.build()

    def package(self):
        if self.settings.os == "Windows":
            self.copy("Agnes.exe", dst="bin", src="bin", keep_path=False, ignore_case=True)
            self.copy("Agnes.exe", dst="bin", src="Source/Preprocessor/Release", keep_path=False, ignore_case=True)
        else:
            self.copy("Agnes", dst="bin", src="bin", keep_path=False, ignore_case=True)
            self.copy("Agnes", dst="bin", src="Source/Preprocessor", keep_path=False, ignore_case=True)
