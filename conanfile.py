from conans import ConanFile, CMake, tools

class AgnesConan(ConanFile):
    name = "Agnes"
    version = "0.1"
    license = "MIT"
    url = "https://github.com/BentouDev/AnnotationGenerator"
    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = ["Modules/*", "Dependencies/*", "Source/*", "CMakeLists.txt"]

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include/Reflection", src="Source/Reflection/include")

        if self.settings.os == "Windows":
            self.copy("Agnes.exe", dst="bin", src="Source/Preprocessor/Release", keep_path=False, ignore_case=True)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self, folder="lib");
