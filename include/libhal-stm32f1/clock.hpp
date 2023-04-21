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

#include <array>

#include <libhal-util/bit.hpp>
#include <libhal-util/enum.hpp>
#include <libhal/config.hpp>

#include "constants.hpp"
#include "internal/reset_and_clock_control.hpp"

namespace hal::stm32f1 {

class clock
{
public:
  /// Constant for the frequency of the LSE
  static constexpr auto low_speed_internal = 20.0_kHz;

  /// Constant for the frequency of the HSE
  static constexpr auto high_speed_internal = 8.0_MHz;

  /// Constant for the frequency of the Flash Controller
  static constexpr auto flash_clock = high_speed_internal;

  /// Constant for the frequency of the Watch Dog Peripheral
  static constexpr auto watchdog_clock_rate = low_speed_internal;

  struct flash_t
  {
    volatile std::uint32_t acr;
    volatile std::uint32_t keyr;
    volatile std::uint32_t optkeyr;
    volatile std::uint32_t sr;
    volatile std::uint32_t cr;
    volatile std::uint32_t ar;
    volatile std::uint32_t reserved;
    volatile std::uint32_t obr;
    volatile std::uint32_t wrpr;
    std::array<uint32_t, 8> reserved1;
    volatile std::uint32_t keyr2;
    uint32_t reserved2;
    volatile std::uint32_t sr2;
    volatile std::uint32_t cr2;
    volatile std::uint32_t ar2;
  };

  /// Pointer to the flash control register
  static flash_t& flash()
  {
    if (hal::is_a_test()) {
      static flash_t test_reg{};
      return test_reg;
    }
    return *reinterpret_cast<flash_t*>(0x4002'2000);
  }

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

  /// Bit masks for the CFGR register
  struct clock_configuration
  {
    /// Controls which clock signal is sent to the MCO pin
    static constexpr auto mco = bit::mask::from<24, 26>();

    /// Sets the USB clock divider
    static constexpr auto usb_prescalar = bit::mask::from<22>();

    /// Sets the PLL multiplier
    static constexpr auto pll_mul = bit::mask::from<18, 21>();

    /// If set to 1, will divide the HSE signal by 2 before sending to PLL
    static constexpr auto hse_pre_divider = bit::mask::from<17>();

    /// Sets which source the PLL will take as input
    static constexpr auto pll_source = bit::mask::from<16>();

    /// Sets the clock divider for the ADC peripherals
    static constexpr auto adc_divider = bit::mask::from<14, 15>();

    /// Sets the divider for peripherals on the APB2 bus
    static constexpr auto apb_2_divider = bit::mask::from<11, 13>();

    /// Sets the divider for peripherals on the APB1 bus
    static constexpr auto apb_1_divider = bit::mask::from<8, 10>();

    /// Sets the divider for peripherals on the AHB bus
    static constexpr auto ahb_divider = bit::mask::from<4, 7>();

    /// Used to check if the system clock has taken the new system clock
    /// settings.
    static constexpr auto system_clock_status = bit::mask::from<2, 3>();

    /// Set which clock will be used for the system clock.
    static constexpr auto system_clock_select = bit::mask::from<0, 1>();

    static auto reg()
    {
      return hal::bit::modify(internal::rcc().cfgr);
    }
  };

  /// Bit masks for the CR register
  struct clock_control
  {
    /// Indicates if the PLL is enabled and ready
    static constexpr auto pll_ready = bit::mask::from<25>();
    /// Used to enable the PLL
    static constexpr auto pll_enable = bit::mask::from<24>();
    /// Indicates if the external oscillator is ready for use
    static constexpr auto external_osc_ready = bit::mask::from<17>();
    /// Used to enable the external oscillator
    static constexpr auto external_osc_enable = bit::mask::from<16>();

    static auto reg()
    {
      return hal::bit::modify(internal::rcc().cr);
    }
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

  /// Bitmasks for the BDCR register
  struct rtc_register
  {
    /// Will reset all clock states for the RTC
    static constexpr auto backup_domain_reset = bit::mask::from<16>();
    /// Enables the RTC clock
    static constexpr auto rtc_enable = bit::mask::from<15>();
    /// Selects the clock source for the RTC
    static constexpr auto rtc_source_select = bit::mask::from<8, 9>();
    /// Indicates if the LSE is ready for use
    static constexpr auto low_speed_osc_ready = bit::mask::from<1>();
    /// Used to enable the LSE
    static constexpr auto low_speed_osc_enable = bit::mask::from<0>();

    static auto reg()
    {
      return hal::bit::modify(internal::rcc().bdcr);
    }
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

  static clock& get()
  {
    static clock system_clock;
    return system_clock;
  }

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
  hal::status reconfigure_clocks()
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
    if (m_config.high_speed_external != 0.0_MHz) {
      clock_control::reg().set(clock_control::external_osc_enable);

      while (!bit::extract<clock_control::external_osc_ready>(
        clock_control::reg().get())) {
        continue;
      }
    }

    // Step 3.2 Enable Low speed external Oscillator
    if (m_config.low_speed_external != 0.0_MHz) {
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
        (m_config.pll.source == pll_source::high_speed_external_divided_by_2))
      .insert<clock_configuration::pll_source>(value(m_config.pll.source));

    // =========================================================================
    // Step 5. Setup PLLs and enable them where necessary
    // =========================================================================
    if (m_config.pll.enable) {
      clock_configuration::reg().insert<clock_configuration::pll_mul>(
        value(m_config.pll.multiply));

      clock_control::reg().set(clock_control::pll_enable);

      while (
        !bit::extract<clock_control::pll_ready>(clock_control::reg().get())) {
        continue;
      }

      switch (m_config.pll.source) {
        case pll_source::high_speed_internal:
          m_pll_clock_rate = high_speed_internal / 2;
          break;
        case pll_source::high_speed_external:
          m_pll_clock_rate = m_config.high_speed_external;
          break;
        case pll_source::high_speed_external_divided_by_2:
          m_pll_clock_rate = m_config.high_speed_external / 2;
          break;
      }

      // Multiply the PLL clock up to the correct rate.
      m_pll_clock_rate = m_pll_clock_rate * (value(m_config.pll.multiply) + 2);
    }

    // =========================================================================
    // Step 6. Setup peripheral dividers
    // =========================================================================
    clock_configuration::reg()
      // Step 6.1 Set USB divider
      .insert<clock_configuration::usb_prescalar>(
        value(m_config.pll.usb.divider))
      // Step 6.2 Set AHB divider
      .insert<clock_configuration::ahb_divider>(value(m_config.ahb.divider))
      // Step 6.3 Set APB1 divider
      .insert<clock_configuration::apb_1_divider>(
        value(m_config.ahb.apb1.divider))
      // Step 6.4 Set APB2 divider
      .insert<clock_configuration::apb_2_divider>(
        value(m_config.ahb.apb2.divider))
      // Step 6.5 Set ADC divider
      .insert<clock_configuration::adc_divider>(
        value(m_config.ahb.apb2.adc.divider));

    // =========================================================================
    // Step 7. Set System Clock and RTC Clock
    // =========================================================================
    uint32_t target_clock_source = value(m_config.system_clock);

    // Step 7.1 Set the Flash wait states appropriately prior to setting the
    //          system clock frequency. Failure to do this will cause the system
    //          to be unable to read from flash, resulting in the platform
    //          locking up. See p.60 of RM0008 for the Flash ACR register
    if (m_config.system_clock == system_clock_select::pll) {
      if (m_pll_clock_rate <= 24.0_MHz) {
        // 0 Wait states
        bit::modify(flash().acr).insert<bit::mask::from<0, 2>()>(0b000U);
      } else if (24.0_MHz <= m_pll_clock_rate && m_pll_clock_rate <= 48.0_MHz) {
        // 1 Wait state
        bit::modify(flash().acr).insert<bit::mask::from<0, 2>()>(0b001U);
      } else {
        // 2 Wait states
        bit::modify(flash().acr).insert<bit::mask::from<0, 2>()>(0b010U);
      }
    }

    // Step 7.2 Set system clock source
    // NOTE: return error if clock = system_clock_select::high_speed_external
    // and
    //       high speed external is not enabled.
    clock_configuration::reg().insert<clock_configuration::system_clock_select>(
      value(m_config.system_clock));

    while (bit::extract<clock_configuration::system_clock_status>(
             clock_configuration::reg().get()) != target_clock_source) {
      continue;
    }

    switch (m_config.system_clock) {
      case system_clock_select::high_speed_internal:
        system_clock = high_speed_internal;
        break;
      case system_clock_select::high_speed_external:
        system_clock = m_config.high_speed_external;
        break;
      case system_clock_select::pll:
        system_clock = m_pll_clock_rate;
        break;
    }

    rtc_register::reg()
      // Step 7.3 Set the RTC oscillator source
      .insert<rtc_register::rtc_source_select>(value(m_config.rtc.source))
      // Step 7.4 Enable/Disable the RTC
      .insert<rtc_register::rtc_enable>(m_config.rtc.enable);

    // =========================================================================
    // Step 8. Define the clock rates for the system
    // =========================================================================
    switch (m_config.ahb.divider) {
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

    switch (m_config.ahb.apb1.divider) {
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

    switch (m_config.ahb.apb2.divider) {
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

    switch (m_config.rtc.source) {
      case rtc_source::no_clock:
        m_rtc_clock_rate = 0.0_Hz;
        break;
      case rtc_source::low_speed_internal:
        m_rtc_clock_rate = low_speed_internal;
        break;
      case rtc_source::low_speed_external:
        m_rtc_clock_rate = m_config.low_speed_external;
        break;
      case rtc_source::high_speed_external_divided_by_128:
        m_rtc_clock_rate = m_config.high_speed_external / 128;
        break;
    }

    switch (m_config.pll.usb.divider) {
      case usb_divider::divide_by_1:
        m_usb_clock_rate = m_pll_clock_rate;
        break;
      case usb_divider::divide_by_1_point_5:
        m_usb_clock_rate = (m_pll_clock_rate * 2) / 3;
        break;
    }

    switch (m_config.ahb.apb1.divider) {
      case apb_divider::divide_by_1:
        m_timer_apb1_clock_rate = m_apb1_clock_rate;
        break;
      default:
        m_timer_apb1_clock_rate = m_apb1_clock_rate * 2;
        break;
    }

    switch (m_config.ahb.apb2.divider) {
      case apb_divider::divide_by_1:
        m_timer_apb2_clock_rate = m_apb2_clock_rate;
        break;
      default:
        m_timer_apb2_clock_rate = m_apb2_clock_rate * 2;
        break;
    }

    switch (m_config.ahb.apb2.adc.divider) {
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

  /**
   * @brief Return a pointer to the clock configuration object
   *
   * @return auto& - clock configuration reference
   */
  auto& config()
  {
    return m_config;
  }

  /// @return the clock rate frequency of a peripheral
  hal::hertz get_frequency(peripheral p_id) const
  {
    switch (p_id) {
      case peripheral::i2s:
        return m_pll_clock_rate;
      case peripheral::usb:
        return m_usb_clock_rate;
      case peripheral::flitf:
        return high_speed_internal;

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
    }

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

private:
  clock_tree m_config{};
  hal::hertz m_rtc_clock_rate = 0.0_Hz;
  hal::hertz m_usb_clock_rate = 0.0_Hz;
  hal::hertz m_pll_clock_rate = 0.0_Hz;
  hal::hertz m_ahb_clock_rate = high_speed_internal;
  hal::hertz m_apb1_clock_rate = 0.0_Hz;
  hal::hertz m_apb2_clock_rate = 0.0_Hz;
  hal::hertz m_timer_apb1_clock_rate = 0.0_Hz;
  hal::hertz m_timer_apb2_clock_rate = 0.0_Hz;
  hal::hertz m_adc_clock_rate = 0.0_Hz;
};
}  // namespace hal::stm32f1
