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

namespace hal::stm32f1 {
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
inline reset_and_clock_control_t* rcc =
  reinterpret_cast<reset_and_clock_control_t*>(0x40000000 + 0x20000 + 0x1000);

/// Bit masks for the CFGR register
struct clock_configuration
{
  /// Controls which clock signal is sent to the MCO pin
  static constexpr auto mco = bit_mask::from<24, 26>();

  /// Sets the USB clock divider
  static constexpr auto usb_prescalar = bit_mask::from<22>();

  /// Sets the PLL multiplier
  static constexpr auto pll_mul = bit_mask::from<18, 21>();

  /// If set to 1, will divide the HSE signal by 2 before sending to PLL
  static constexpr auto hse_pre_divider = bit_mask::from<17>();

  /// Sets which source the PLL will take as input
  static constexpr auto pll_source = bit_mask::from<16>();

  /// Sets the clock divider for the ADC peripherals
  static constexpr auto adc_divider = bit_mask::from<14, 15>();

  /// Sets the divider for peripherals on the APB2 bus
  static constexpr auto apb_2_divider = bit_mask::from<11, 13>();

  /// Sets the divider for peripherals on the APB1 bus
  static constexpr auto apb_1_divider = bit_mask::from<8, 10>();

  /// Sets the divider for peripherals on the AHB bus
  static constexpr auto ahb_divider = bit_mask::from<4, 7>();

  /// Used to check if the system clock has taken the new system clock
  /// settings.
  static constexpr auto system_clock_status = bit_mask::from<2, 3>();

  /// Set which clock will be used for the system clock.
  static constexpr auto system_clock_select = bit_mask::from<0, 1>();

  static auto reg()
  {
    return hal::bit_modify(rcc->cfgr);
  }
};

/// Bit masks for the CR register
struct clock_control
{
  /// Indicates if the PLL is enabled and ready
  static constexpr auto pll_ready = bit_mask::from<25>();
  /// Used to enable the PLL
  static constexpr auto pll_enable = bit_mask::from<24>();
  /// Indicates if the external oscillator is ready for use
  static constexpr auto external_osc_ready = bit_mask::from<17>();
  /// Used to enable the external oscillator
  static constexpr auto external_osc_enable = bit_mask::from<16>();

  static auto reg()
  {
    return hal::bit_modify(rcc->cr);
  }
};

/// Bitmasks for the BDCR register
struct rtc_register
{
  /// Will reset all clock states for the RTC
  static constexpr auto backup_domain_reset = bit_mask::from<16>();
  /// Enables the RTC clock
  static constexpr auto rtc_enable = bit_mask::from<15>();
  /// Selects the clock source for the RTC
  static constexpr auto rtc_source_select = bit_mask::from<8, 9>();
  /// Indicates if the LSE is ready for use
  static constexpr auto low_speed_osc_ready = bit_mask::from<1>();
  /// Used to enable the LSE
  static constexpr auto low_speed_osc_enable = bit_mask::from<0>();

  static auto reg()
  {
    return hal::bit_modify(rcc->bdcr);
  }
};
}  // namespace hal::stm32f1
