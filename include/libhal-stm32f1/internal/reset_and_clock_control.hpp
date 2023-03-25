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
