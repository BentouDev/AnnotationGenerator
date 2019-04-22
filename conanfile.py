from conans import ConanFile, CMake, tools
import os

agnes_version = os.getenv('AGNES_VERSION', '0.0')
agnes_commit = os.getenv('AGNES_COMMIT', '')

class AgnesConan(ConanFile):
    name = "Agnes"
    license = "MIT"
    url = "https://github.com/BentouDev/AnnotationGenerator"
    version = agnes_version
    commit = agnes_commit

    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = ["Modules/*", "Dependencies/*", "Source/*", "CMakeLists.txt"]

    build_requires = [
        "jsonformoderncpp/3.6.1@vthiery/stable",
        "kainjow-mustache/3.2.1@bincrafters/stable"
    ]

    def package_id(self):
        self.info.include_build_settings()
        del self.info.settings.compiler
        del self.info.settings.arch
        del self.info.settings.build_type

    def build(self):
        cmake = CMake(self)
        cmake.definitions['AGNES_VERSION'] = self.version
        cmake.definitions['AGNES_COMMIT'] = self.commit
        cmake.definitions['AGNES_CHANNEL'] = self.channel
        cmake.configure(source_folder=".")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        if self.settings.os == "Windows":
            self.copy("Agnes.exe", dst="bin", src="Source/Preprocessor/Release", keep_path=False, ignore_case=True)
        else:
            self.copy("Agnes", dst="bin", src="Source/Preprocessor", keep_path=False, ignore_case=True)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self, folder="lib")
