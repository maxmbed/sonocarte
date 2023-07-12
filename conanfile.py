import os
from conan import ConanFile

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("libsndfile/1.2.0")

        # Cannot add libalsa in the requirement list
        # The runtime environement variable ALSA_CONFIG_DIR is linked to conan sub-directory instead of the system one
        # See known issue: https://github.com/conan-io/conan-center-index/issues/2474
        #self.requires("libalsa/1.2.7.2")

    def build_requirements(self):
        self.tool_requires("cmake/3.26.4")
