from conans import ConanFile, CMake, tools


class MultiformatsConan(ConanFile):
    name = "multiformats"
    version = "0.1"
    license = "MIT"
    author = "Matthew Knight mgk1795@gmail.com"
    url = "https://github.com/matt1795/multiformats"
    description = "C++"
    homepage = "https://multiformats.io/"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    export_sources = "*"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self.source_folder)
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="hello")
        self.copy("*hello.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.cppflags = ["-std-c++17"]
