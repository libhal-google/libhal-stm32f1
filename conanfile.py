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
from conan.errors import ConanInvalidConfiguration
import os

required_conan_version = ">=1.50.0"


class libhal_stm32f1_conan(ConanFile):
    name = "libhal-stm32f1"
    version = "0.3.0"
    license = "Apache-2.0"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://libhal.github.io/libhal-stm32f1"
    description = ("Drivers for the stm32f1 series of microcontrollers using "
                   "libhal's abstractions.")
    topics = ("ARM", "microcontroller", "peripherals", "hardware", "stm32f1")
    settings = "compiler", "build_type", "os", "arch"
    exports_sources = "include/*", "tests/*",  "linker_scripts/*", "LICENSE"
    generators = "CMakeToolchain", "CMakeDeps"
    no_copy_source = True

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

    def validate(self):
        if self.settings.get_safe("compiler.cppstd"):
            check_min_cppstd(self, self._min_cppstd)

        def lazy_lt_semver(v1, v2):
            lv1 = [int(v) for v in v1.split(".")]
            lv2 = [int(v) for v in v2.split(".")]
            min_length = min(len(lv1), len(lv2))
            return lv1[:min_length] < lv2[:min_length]

        compiler = str(self.settings.compiler)
        version = str(self.settings.compiler.version)
        minimum_version = self._compilers_minimum_version.get(compiler, False)

        if minimum_version and lazy_lt_semver(version, minimum_version):
            raise ConanInvalidConfiguration(
                f"{self.name} {self.version} requires C++{self._min_cppstd}, which your compiler ({compiler}-{version}) does not support")

    def requirements(self):
        self.requires("libhal/[^1.0.0]")
        self.requires("libhal-util/[^1.0.0]")
        self.requires("libhal-armcortex/[^1.0.0]")
        self.test_requires("boost-ext-ut/1.1.9")

    def layout(self):
        cmake_layout(self)

    def build(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake = CMake(self)
            if self.settings.os == "Windows":
                cmake.configure(build_script_folder="tests")
            else:
                cmake.configure(build_script_folder="tests",
                                variables={"ENABLE_ASAN": True})
            cmake.build()
            self.run(os.path.join(self.cpp.build.bindir, "unit_test"))

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

    def package_info(self):
        requirements_list = ["libhal::libhal",
                             "libhal-util::libhal-util",
                             "libhal-armcortex::libhal-armcortex"]

        m3_architecture_flags = ["-mcpu=cortex-m3", "-mfloat-abi=soft"]

        linker_path = os.path.join(self.package_folder, "linker_scripts")

        self.cpp_info.set_property("cmake_file_name", "libhal-stm32f1")
        self.cpp_info.set_property("cmake_find_mode", "both")

        self.cpp_info.components["stm32f1"].set_property(
            "cmake_target_name",  "libhal::stm32f1")
        self.cpp_info.components["stm32f1"].exelinkflags.append(
            "-L" + linker_path)
        self.cpp_info.components["stm32f1"].requires = requirements_list

        def create_component(self, component, flags):
            link_script = "-Tlibhal-stm32f1/" + component + ".ld"
            component_name = "libhal::" + component
            self.cpp_info.components[component].set_property(
                "cmake_target_name", component_name)
            self.cpp_info.components[component].requires = ["stm32f1"]
            self.cpp_info.components[component].exelinkflags.append(link_script)
            self.cpp_info.components[component].exelinkflags.extend(flags)
            self.cpp_info.components[component].cflags = flags
            self.cpp_info.components[component].cxxflags = flags

        create_component(self, "stm32f103", m3_architecture_flags)

    def package_id(self):
        self.info.clear()
