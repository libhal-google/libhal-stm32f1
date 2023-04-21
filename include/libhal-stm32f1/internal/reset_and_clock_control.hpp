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

#include <cstdint>

#include <libhal/config.hpp>

namespace hal::stm32f1::internal {
struct reset_and_clock_control_t
{
  volatile std::uint32_t cr;
  volatile std::uint32_t cfgr;
  volatile std::uint32_t cir;
  volatile std::uint32_t apb2rstr;
  volatile std::uint32_t apb1rstr;
  volatile std::uint32_t ahbenr;
  volatile std::uint32_t apb2enr;
  volatile std::uint32_t apb1enr;
  volatile std::uint32_t bdcr;
  volatile std::uint32_t csr;
  volatile std::uint32_t ahbrstr;
  volatile std::uint32_t cfgr2;
};

/**
 * @return reset_and_clock_control_t& - return reset_and_clock_control_t
 * register.
 */
inline reset_and_clock_control_t& rcc()
{
  if constexpr (hal::is_a_test()) {
    static reset_and_clock_control_t test_reg{};
    return test_reg;
  }
  return *reinterpret_cast<reset_and_clock_control_t*>(0x40000000 + 0x20000 +
                                                       0x1000);
}
}  // namespace hal::stm32f1::internal
