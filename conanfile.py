from conans import ConanFile, CMake, tools

class AgnesConan(ConanFile):
    name = "Agnes"
    license = "MIT"

    description = "AnnotationGenerator conan package"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = ["Modules/*", "Dependencies/*", "Source/*", "CMakeLists.txt"]

    git = tools.Git()
    print("Git: branch - " + git.get_branch())
    print("     tag    - " + git.get_tag())
    print("     commit - " + git.get_commit())

    print("path: " + self.env_info.path)
    print("CI: " + self.env_info['CI'])
    print("APP_VEYOR: " + self.env_info['APPVEYOR'])
    print("      tag: " + self.env_info['APPVEYOR_REPO_TAG'])
    print("TRAVIS: " + self.env_info['TRAVIS'])
    print("   tag: " + self.env_info['TRAVIS_TAG'])

    def package_id(self):
        self.info.include_build_settings()
        del self.info.settings.compiler
        del self.info.settings.arch
        del self.info.settings.build_type

    def build(self):
        cmake = CMake(self)
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
