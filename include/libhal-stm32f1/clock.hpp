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

#include "constants.hpp"

#include <libhal/error.hpp>
#include <libhal/units.hpp>

namespace hal::stm32f1 {

using namespace hal::literals;

/// Constant for the frequency of the LSE
constexpr auto internal_low_speed_oscillator = 20.0_kHz;

/// Constant for the frequency of the HSE
constexpr auto internal_high_speed_oscillator = 8.0_MHz;

/// Constant for the frequency of the Flash Controller
constexpr auto flash_clock = internal_high_speed_oscillator;

/// Constant for the frequency of the Watch Dog Peripheral
constexpr auto watchdog_clock_rate = internal_low_speed_oscillator;

/// Available dividers for the APB bus
enum class apb_divider : std::uint8_t
{
  divide_by_1 = 0,
  divide_by_2 = 0b100,
  divide_by_4 = 0b101,
  divide_by_8 = 0b110,
  divide_by_16 = 0b111,
};

/// Available dividers for the AHB bus
enum class ahb_divider : std::uint8_t
{
  divide_by_1 = 0,
  divide_by_2 = 0b1000,
  divide_by_4 = 0b1001,
  divide_by_8 = 0b1010,
  divide_by_16 = 0b1011,
  divide_by_64 = 0b1100,
  divide_by_128 = 0b1101,
  divide_by_256 = 0b1110,
  divide_by_512 = 0b1111,
};

/// Available dividers for the ADC bus
enum class adc_divider : std::uint8_t
{
  divide_by_2 = 0b00,
  divide_by_4 = 0b01,
  divide_by_6 = 0b10,
  divide_by_8 = 0b11,
};

/// Available clock sources available for the system clock
enum class system_clock_select : std::uint8_t
{
  high_speed_internal = 0b00,
  high_speed_external = 0b01,
  pll = 0b10,
};

/// PLL frequency multiplication options.
enum class pll_multiply : std::uint8_t
{
  multiply_by_2 = 0b0000,
  multiply_by_3 = 0b0001,
  multiply_by_4 = 0b0010,
  multiply_by_5 = 0b0011,
  multiply_by_6 = 0b0100,
  multiply_by_7 = 0b0101,
  multiply_by_8 = 0b0110,
  multiply_by_9 = 0b0111,
  multiply_by_10 = 0b1000,
  multiply_by_11 = 0b1001,
  multiply_by_12 = 0b1010,
  multiply_by_13 = 0b1011,
  multiply_by_14 = 0b1100,
  multiply_by_15 = 0b1101,
  multiply_by_16 = 0b1110,
};

/// Available clock sources for the RTC
enum class rtc_source : std::uint8_t
{
  no_clock = 0b00,
  low_speed_internal = 0b01,
  low_speed_external = 0b10,
  high_speed_external_divided_by_128 = 0b11,
};

/// Available clock sources for the PLL
enum class pll_source : std::uint8_t
{
  high_speed_internal = 0b0,
  high_speed_external = 0b1,
  high_speed_external_divided_by_2 = 0b11,
};

/// Available dividers for the USB peripheral
enum class usb_divider : std::uint8_t
{
  divide_by_1 = 1,
  /// Divide by 1.5
  divide_by_1_point_5 = 0,
};

struct clock_tree
{
  /// Defines the frequency of the high speed external clock signal
  hal::hertz high_speed_external = 0.0_MHz;

  /// Defines the frequency of the low speed external clock signal.
  hal::hertz low_speed_external = 0.0_MHz;

  /// Defines the configuration of the PLL
  struct pll_t
  {
    bool enable = false;
    pll_source source = pll_source::high_speed_internal;
    pll_multiply multiply = pll_multiply::multiply_by_2;
    struct usb_divider_t
    {
      usb_divider divider = usb_divider::divide_by_1_point_5;
    } usb = {};
  } pll = {};

  /// Defines which clock source will be use for the system.
  /// @warning System will lock up in the following situations:
  ///          - Select PLL, but PLL is not enabled
  ///          - Select PLL, but PLL frequency is too high
  ///          - Select High Speed External, but the frequency is kept at
  ///            0_Mhz.
  system_clock_select system_clock = system_clock_select::high_speed_internal;

  /// Defines the configuration for the RTC
  struct rtc_t
  {
    bool enable = false;
    rtc_source source = rtc_source::low_speed_internal;
  } rtc = {};

  /// Defines the configuration of the dividers beyond system clock mux.
  struct ahb_t
  {
    ahb_divider divider = ahb_divider::divide_by_1;
    /// Maximum rate of 36 MHz
    struct apb1_t
    {
      apb_divider divider = apb_divider::divide_by_1;
    } apb1 = {};

    struct apb2_t
    {
      apb_divider divider = apb_divider::divide_by_1;
      struct adc_t
      {
        /// Maximum of 14 MHz
        adc_divider divider = adc_divider::divide_by_2;
      } adc = {};
    } apb2 = {};
  } ahb = {};
};

/// @attention If configuration of the system clocks is desired, one should
///            consult the user manual of the target MCU in use to determine
///            the valid clock configuration values that can/should be used.
///            The Initialize() method is only responsible for configuring the
///            clock system based on configurations in the
///            clock_configuration. Incorrect configurations may result in a
///            hard fault or cause the clock system(s) to supply incorrect
///            clock rate(s).
///
/// @see Figure 11. Clock Tree
///      https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=126
void configure_clocks(clock_tree p_clock_tree);

/// @return the clock rate frequency of a peripheral
hal::hertz frequency(peripheral p_id);
}  // namespace hal::stm32f1
