#!/usr/bin/python
#
# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy
from conan.tools.build import check_min_cppstd
import os


required_conan_version = ">=2.0.6"


class libhal_stm32f1_conan(ConanFile):
    name = "libhal-stm32f1"
    version = "2.0.3"
    license = "Apache-2.0"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://libhal.github.io/libhal-stm32f1"
    description = ("A collection of drivers and libraries for the stm32f1 "
                   "series microcontrollers from NXP")
    topics = ("arm", "microcontroller", "lpc", "stm32f1",
              "stm32f1xx", "stm32f172", "stm32f174", "stm32f178", "stm32f188")
    settings = "compiler", "build_type", "os", "arch"
    exports_sources = "include/*", "linker_scripts/*", "tests/*", "LICENSE", "CMakeLists.txt", "src/*"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualBuildEnv"

    options = {
        "platform": ["ANY"]
    }
    default_options = {
        "platform": "ANY"
    }

    @property
    def _is_me(self):
        return (
            str(self.options.platform).startswith("stm32f1") and
            len(str(self.options.platform)) == 11
        )

    @property
    def _min_cppstd(self):
        return "20"

    @property
    def _compilers_minimum_version(self):
        return {
            "gcc": "11",
            "clang": "14",
            "apple-clang": "14.0.0"
        }

    @property
    def _bare_metal(self):
        return self.settings.os == "baremetal"

    def package_id(self):
        if self.info.options.get_safe("platform"):
            del self.info.options.platform

    def validate(self):
        if self.settings.get_safe("compiler.cppstd"):
            check_min_cppstd(self, self._min_cppstd)

    def build_requirements(self):
        self.tool_requires("libhal-cmake-util/3.0.0")
        self.test_requires("boost-ext-ut/1.1.9")

    def requirements(self):
        self.requires("libhal/[^2.0.0]")
        self.requires("libhal-util/[^3.0.0]")
        self.requires("libhal-armcortex/[^2.0.3]")

    def layout(self):
        cmake_layout(self)

    def build(self):
        run_test = not self.conf.get("tools.build:skip_test", default=False)

        cmake = CMake(self)
        if self.settings.os == "Windows":
            cmake.configure()
        elif self._bare_metal:
            cmake.configure(variables={
                "BUILD_TESTING": "OFF"
            })
        else:
            cmake.configure(variables={"ENABLE_ASAN": True})

        cmake.build()

        if run_test and not self._bare_metal:
            test_folder = os.path.join("tests")
            self.run(os.path.join(test_folder, "unit_test"))

    def package(self):
        copy(self,
             "LICENSE",
             dst=os.path.join(self.package_folder, "licenses"),
             src=self.source_folder)
        copy(self,
             "*.h",
             dst=os.path.join(self.package_folder, "include"),
             src=os.path.join(self.source_folder, "include"))
        copy(self,
             "*.hpp",
             dst=os.path.join(self.package_folder, "include"),
             src=os.path.join(self.source_folder, "include"))
        copy(self,
             "*.ld",
             dst=os.path.join(self.package_folder, "linker_scripts"),
             src=os.path.join(self.source_folder, "linker_scripts"))

        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_target_name", "libhal::stm32f1")
        self.cpp_info.libs = ["libhal-stm32f1"]

        if self._bare_metal and self._is_me:
            linker_script_name = list(str(self.options.platform))
            # Replace the MCU number and pin count number with 'x' (don't care)
            # to map to the linker script
            linker_script_name[8] = 'x'
            linker_script_name[9] = 'x'
            linker_script_name = "".join(linker_script_name)

            linker_path = os.path.join(self.package_folder, "linker_scripts")
            link_script = "-Tlibhal-stm32f1/" + linker_script_name + ".ld"
            self.cpp_info.exelinkflags = ["-L" + linker_path, link_script]
