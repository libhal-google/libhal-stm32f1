# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.15)

project(libhal-stm32f1 VERSION 0.0.1 LANGUAGES CXX)

find_package(libhal REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)
find_package(libhal-armcortex REQUIRED CONFIG)

add_library(libhal-stm32f1
  src/power.cpp
  src/clock.cpp
  src/pin.cpp
  src/output_pin.cpp
  src/spi.cpp)

target_include_directories(libhal-stm32f1 PUBLIC include)
target_compile_features(libhal-stm32f1 PRIVATE cxx_std_20)
target_compile_options(libhal-stm32f1 PRIVATE -g)
target_link_libraries(libhal-stm32f1 PUBLIC
  libhal::libhal
  libhal::util
  libhal::armcortex
)

if(NOT BUILD_TESTING STREQUAL OFF)
  add_subdirectory(tests)
endif()

install(TARGETS libhal-stm32f1)
