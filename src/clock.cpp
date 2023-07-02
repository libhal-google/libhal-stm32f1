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

#include <libhal-stm32f1/clock.hpp>

#include <array>

#include <libhal-stm32f1/constants.hpp>
#include <libhal-util/bit.hpp>
#include <libhal-util/enum.hpp>

#include "flash_reg.hpp"
#include "rcc_reg.hpp"

namespace hal::stm32f1 {

namespace {
hal::hertz m_rtc_clock_rate = 0.0_Hz;
hal::hertz m_usb_clock_rate = 0.0_Hz;
hal::hertz m_pll_clock_rate = 0.0_Hz;
hal::hertz m_ahb_clock_rate = internal_high_speed_oscillator;
hal::hertz m_apb1_clock_rate = 0.0_Hz;
hal::hertz m_apb2_clock_rate = 0.0_Hz;
hal::hertz m_timer_apb1_clock_rate = 0.0_Hz;
hal::hertz m_timer_apb2_clock_rate = 0.0_Hz;
hal::hertz m_adc_clock_rate = 0.0_Hz;
}  // namespace

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
void configure_clocks(clock_tree p_clock_tree)
{
  hal::hertz system_clock = 0.0_Hz;

  // =========================================================================
  // Step 1. Select internal clock source for everything.
  //         Make sure PLLs are not clock sources for everything.
  // =========================================================================
  // Step 1.1 Set SystemClock to HSI
  clock_configuration::reg().insert<clock_configuration::system_clock_select>(
    value(system_clock_select::high_speed_internal));

  // Step 1.4 Reset RTC clock registers
  rtc_register::reg().set(rtc_register::backup_domain_reset);

  // Manually clear the RTC reset bit
  rtc_register::reg().clear(rtc_register::backup_domain_reset);

  // =========================================================================
  // Step 2. Disable PLL and external clock sources
  // =========================================================================
  clock_control::reg()
    // Step 2.1 Disable PLLs
    .clear(clock_control::pll_enable)
    // Step 2.1 Disable External Oscillators
    .clear(clock_control::external_osc_enable);

  // =========================================================================
  // Step 3. Enable External Oscillators
  // =========================================================================
  // Step 3.1 Enable High speed external Oscillator
  if (p_clock_tree.high_speed_external != 0.0_MHz) {
    clock_control::reg().set(clock_control::external_osc_enable);

    while (!bit::extract<clock_control::external_osc_ready>(
      clock_control::reg().get())) {
      continue;
    }
  }

  // Step 3.2 Enable Low speed external Oscillator
  if (p_clock_tree.low_speed_external != 0.0_MHz) {
    rtc_register::reg().set(rtc_register::low_speed_osc_enable);

    while (!bit::extract<rtc_register::low_speed_osc_ready>(
      rtc_register::reg().get())) {
      continue;
    }
  }

  // =========================================================================
  // Step 4. Set oscillator source for PLLs
  // =========================================================================
  clock_configuration::reg()
    .insert<clock_configuration::hse_pre_divider>(
      (p_clock_tree.pll.source == pll_source::high_speed_external_divided_by_2))
    .insert<clock_configuration::pll_source>(value(p_clock_tree.pll.source));

  // =========================================================================
  // Step 5. Setup PLLs and enable them where necessary
  // =========================================================================
  if (p_clock_tree.pll.enable) {
    clock_configuration::reg().insert<clock_configuration::pll_mul>(
      value(p_clock_tree.pll.multiply));

    clock_control::reg().set(clock_control::pll_enable);

    while (
      !bit::extract<clock_control::pll_ready>(clock_control::reg().get())) {
      continue;
    }

    switch (p_clock_tree.pll.source) {
      case pll_source::high_speed_internal:
        m_pll_clock_rate = internal_high_speed_oscillator / 2;
        break;
      case pll_source::high_speed_external:
        m_pll_clock_rate = p_clock_tree.high_speed_external;
        break;
      case pll_source::high_speed_external_divided_by_2:
        m_pll_clock_rate = p_clock_tree.high_speed_external / 2;
        break;
    }

    // Multiply the PLL clock up to the correct rate.
    m_pll_clock_rate =
      m_pll_clock_rate * (value(p_clock_tree.pll.multiply) + 2);
  }

  // =========================================================================
  // Step 6. Setup peripheral dividers
  // =========================================================================
  clock_configuration::reg()
    // Step 6.1 Set USB divider
    .insert<clock_configuration::usb_prescalar>(
      value(p_clock_tree.pll.usb.divider))
    // Step 6.2 Set AHB divider
    .insert<clock_configuration::ahb_divider>(value(p_clock_tree.ahb.divider))
    // Step 6.3 Set APB1 divider
    .insert<clock_configuration::apb_1_divider>(
      value(p_clock_tree.ahb.apb1.divider))
    // Step 6.4 Set APB2 divider
    .insert<clock_configuration::apb_2_divider>(
      value(p_clock_tree.ahb.apb2.divider))
    // Step 6.5 Set ADC divider
    .insert<clock_configuration::adc_divider>(
      value(p_clock_tree.ahb.apb2.adc.divider));

  // =========================================================================
  // Step 7. Set System Clock and RTC Clock
  // =========================================================================
  uint32_t target_clock_source = value(p_clock_tree.system_clock);

  // Step 7.1 Set the Flash wait states appropriately prior to setting the
  //          system clock frequency. Failure to do this will cause the system
  //          to be unable to read from flash, resulting in the platform
  //          locking up. See p.60 of RM0008 for the Flash ACR register
  if (p_clock_tree.system_clock == system_clock_select::pll) {
    if (m_pll_clock_rate <= 24.0_MHz) {
      // 0 Wait states
      bit::modify(flash->acr).insert<bit::mask::from<0, 2>()>(0b000U);
    } else if (24.0_MHz <= m_pll_clock_rate && m_pll_clock_rate <= 48.0_MHz) {
      // 1 Wait state
      bit::modify(flash->acr).insert<bit::mask::from<0, 2>()>(0b001U);
    } else {
      // 2 Wait states
      bit::modify(flash->acr).insert<bit::mask::from<0, 2>()>(0b010U);
    }
  }

  // Step 7.2 Set system clock source
  // NOTE: return error if clock = system_clock_select::high_speed_external
  // and
  //       high speed external is not enabled.
  clock_configuration::reg().insert<clock_configuration::system_clock_select>(
    value(p_clock_tree.system_clock));

  while (bit::extract<clock_configuration::system_clock_status>(
           clock_configuration::reg().get()) != target_clock_source) {
    continue;
  }

  switch (p_clock_tree.system_clock) {
    case system_clock_select::high_speed_internal:
      system_clock = internal_high_speed_oscillator;
      break;
    case system_clock_select::high_speed_external:
      system_clock = p_clock_tree.high_speed_external;
      break;
    case system_clock_select::pll:
      system_clock = m_pll_clock_rate;
      break;
  }

  rtc_register::reg()
    // Step 7.3 Set the RTC oscillator source
    .insert<rtc_register::rtc_source_select>(value(p_clock_tree.rtc.source))
    // Step 7.4 Enable/Disable the RTC
    .insert<rtc_register::rtc_enable>(p_clock_tree.rtc.enable);

  // =========================================================================
  // Step 8. Define the clock rates for the system
  // =========================================================================
  switch (p_clock_tree.ahb.divider) {
    case ahb_divider::divide_by_1:
      m_ahb_clock_rate = system_clock / 1;
      break;
    case ahb_divider::divide_by_2:
      m_ahb_clock_rate = system_clock / 2;
      break;
    case ahb_divider::divide_by_4:
      m_ahb_clock_rate = system_clock / 4;
      break;
    case ahb_divider::divide_by_8:
      m_ahb_clock_rate = system_clock / 8;
      break;
    case ahb_divider::divide_by_16:
      m_ahb_clock_rate = system_clock / 16;
      break;
    case ahb_divider::divide_by_64:
      m_ahb_clock_rate = system_clock / 64;
      break;
    case ahb_divider::divide_by_128:
      m_ahb_clock_rate = system_clock / 128;
      break;
    case ahb_divider::divide_by_256:
      m_ahb_clock_rate = system_clock / 256;
      break;
    case ahb_divider::divide_by_512:
      m_ahb_clock_rate = system_clock / 512;
      break;
  }

  switch (p_clock_tree.ahb.apb1.divider) {
    case apb_divider::divide_by_1:
      m_apb1_clock_rate = m_ahb_clock_rate / 1;
      break;
    case apb_divider::divide_by_2:
      m_apb1_clock_rate = m_ahb_clock_rate / 2;
      break;
    case apb_divider::divide_by_4:
      m_apb1_clock_rate = m_ahb_clock_rate / 4;
      break;
    case apb_divider::divide_by_8:
      m_apb1_clock_rate = m_ahb_clock_rate / 8;
      break;
    case apb_divider::divide_by_16:
      m_apb1_clock_rate = m_ahb_clock_rate / 16;
      break;
  }

  switch (p_clock_tree.ahb.apb2.divider) {
    case apb_divider::divide_by_1:
      m_apb2_clock_rate = m_ahb_clock_rate / 1;
      break;
    case apb_divider::divide_by_2:
      m_apb2_clock_rate = m_ahb_clock_rate / 2;
      break;
    case apb_divider::divide_by_4:
      m_apb2_clock_rate = m_ahb_clock_rate / 4;
      break;
    case apb_divider::divide_by_8:
      m_apb2_clock_rate = m_ahb_clock_rate / 8;
      break;
    case apb_divider::divide_by_16:
      m_apb2_clock_rate = m_ahb_clock_rate / 16;
      break;
  }

  switch (p_clock_tree.rtc.source) {
    case rtc_source::no_clock:
      m_rtc_clock_rate = 0.0_Hz;
      break;
    case rtc_source::low_speed_internal:
      m_rtc_clock_rate = internal_low_speed_oscillator;
      break;
    case rtc_source::low_speed_external:
      m_rtc_clock_rate = p_clock_tree.low_speed_external;
      break;
    case rtc_source::high_speed_external_divided_by_128:
      m_rtc_clock_rate = p_clock_tree.high_speed_external / 128;
      break;
  }

  switch (p_clock_tree.pll.usb.divider) {
    case usb_divider::divide_by_1:
      m_usb_clock_rate = m_pll_clock_rate;
      break;
    case usb_divider::divide_by_1_point_5:
      m_usb_clock_rate = (m_pll_clock_rate * 2) / 3;
      break;
  }

  switch (p_clock_tree.ahb.apb1.divider) {
    case apb_divider::divide_by_1:
      m_timer_apb1_clock_rate = m_apb1_clock_rate;
      break;
    default:
      m_timer_apb1_clock_rate = m_apb1_clock_rate * 2;
      break;
  }

  switch (p_clock_tree.ahb.apb2.divider) {
    case apb_divider::divide_by_1:
      m_timer_apb2_clock_rate = m_apb2_clock_rate;
      break;
    default:
      m_timer_apb2_clock_rate = m_apb2_clock_rate * 2;
      break;
  }

  switch (p_clock_tree.ahb.apb2.adc.divider) {
    case adc_divider::divide_by_2:
      m_adc_clock_rate = m_apb2_clock_rate / 2;
      break;
    case adc_divider::divide_by_4:
      m_adc_clock_rate = m_apb2_clock_rate / 4;
      break;
    case adc_divider::divide_by_6:
      m_adc_clock_rate = m_apb2_clock_rate / 6;
      break;
    case adc_divider::divide_by_8:
      m_adc_clock_rate = m_apb2_clock_rate / 8;
      break;
  }
}

/// @return the clock rate frequency of a peripheral
hal::hertz frequency(peripheral p_id)
{
  switch (p_id) {
    case peripheral::i2s:
      return m_pll_clock_rate;
    case peripheral::usb:
      return m_usb_clock_rate;
    case peripheral::flitf:
      return internal_high_speed_oscillator;

    // Arm Cortex running clock rate.
    // This code does not utilize the /8 clock for the system timer, thus the
    // clock rate for that subsystem is equal to the CPU running clock.
    case peripheral::system_timer:
      [[fallthrough]];
    case peripheral::cpu:
      return m_ahb_clock_rate;

    // APB1 Timers
    case peripheral::timer2:
      [[fallthrough]];
    case peripheral::timer3:
      [[fallthrough]];
    case peripheral::timer4:
      [[fallthrough]];
    case peripheral::timer5:
      [[fallthrough]];
    case peripheral::timer6:
      [[fallthrough]];
    case peripheral::timer7:
      [[fallthrough]];
    case peripheral::timer12:
      [[fallthrough]];
    case peripheral::timer13:
      [[fallthrough]];
    case peripheral::timer14:
      return m_timer_apb1_clock_rate;

    // APB2 Timers
    case peripheral::timer1:
      [[fallthrough]];
    case peripheral::timer8:
      [[fallthrough]];
    case peripheral::timer9:
      [[fallthrough]];
    case peripheral::timer10:
      [[fallthrough]];
    case peripheral::timer11:
      return m_timer_apb2_clock_rate;

    case peripheral::adc1:
      [[fallthrough]];
    case peripheral::adc2:
      [[fallthrough]];
    case peripheral::adc3:
      return m_adc_clock_rate;
    default: {
      auto id = value(p_id);

      if (id < apb1_bus) {
        return m_ahb_clock_rate;
      }

      if (apb1_bus <= id && id < apb2_bus) {
        return m_apb1_clock_rate;
      }

      if (apb2_bus <= id && id < beyond_bus) {
        return m_apb2_clock_rate;
      }

      return 0.0_Hz;
    }
  }

  return 0.0_Hz;
}
}  // namespace hal::stm32f1
