// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <libhal/config.hpp>

namespace hal::stm32f1 {
constexpr void compile_time_platform_check()
{
  static_assert(hal::is_platform("stm32f10") || hal::is_a_test(),
                "This driver can only be used for stm32f10x series "
                "microcontrollers or for unit tests.");
}
}  // namespace hal::stm32f1
