#pragma once

#include <cstdint>

#include <libhal/config.hpp>

namespace hal::stm32::f10x::internal {
struct reset_and_clock_control_t
{
  volatile std::uint32_t CR;
  volatile std::uint32_t CFGR;
  volatile std::uint32_t CIR;
  volatile std::uint32_t APB2RSTR;
  volatile std::uint32_t APB1RSTR;
  volatile std::uint32_t AHBENR;
  volatile std::uint32_t APB2ENR;
  volatile std::uint32_t APB1ENR;
  volatile std::uint32_t BDCR;
  volatile std::uint32_t CSR;
  volatile std::uint32_t AHBRSTR;
  volatile std::uint32_t CFGR2;
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
}  // namespace hal::stm32::f10x::internal
