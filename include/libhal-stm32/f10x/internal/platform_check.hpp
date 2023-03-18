#pragma once

#include <libhal/config.hpp>

namespace hal::stm32::f10x {
constexpr void compile_time_platform_check()
{
  static_assert(hal::is_platform("stm32f10") || hal::is_a_test(),
                "This driver can only be used for stm32f10x series "
                "microcontrollers or for unit tests.");
}
}  // namespace hal::stm32::f10x
