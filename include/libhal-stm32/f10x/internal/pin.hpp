#pragma once

#include <array>
#include <cstdint>

#include <libhal-util/bit.hpp>
#include <libhal/config.hpp>

#include "../power.hpp"
#include "platform_check.hpp"

namespace hal::stm32::f10x::internal {
/**
 * @brief Check the bounds of a GPIO at compile time and generate a compiler
 * error if the pin and port combination are not supported.
 *
 * @tparam port - selects pin port to use
 * @tparam pin - selects pin within the port to use
 */
template<std::uint8_t port, std::uint8_t pin>
constexpr void check_gpio_bounds_at_compile()
{
  compile_time_platform_check();

  static_assert(
    ('A' <= port && port <= 'E' && 0 <= pin && pin <= 15),
    "For ports between 'A' and 'E', the pin number must be between 0 and 15.");
}

class pin
{
public:
  /**
   * @brief Alternate Function I/O Register Map
   */
  struct alternative_function_io_t
  {
    volatile std::uint32_t evcr;
    volatile std::uint32_t mapr;
    volatile std::array<std::uint32_t, 4> exticr;
    std::uint32_t reserved0;
    volatile std::uint32_t mapr2;
  };

  /**
   * @brief GPIO register map
   *
   */
  struct gpio_t
  {
    volatile std::uint32_t crl;
    volatile std::uint32_t crh;
    volatile std::uint32_t idr;
    volatile std::uint32_t odr;
    volatile std::uint32_t bsrr;
    volatile std::uint32_t brr;
    volatile std::uint32_t lckr;
  };

  /**
   * @brief Map the CONFIG flags for each pin use case
   *
   */
  struct pin_config_t
  {
    /// Configuration bit 1
    std::uint8_t CNF1;
    /// Configuration bit 0
    std::uint8_t CNF0;
    /// Mode bits
    std::uint8_t MODE;
    /// Output data register
    std::uint8_t PxODR;
  };

  /**
   * @brief Returns a reference to the alternative I/O register
   *
   * In unit tests returns a statically allocated structure
   *
   * @return alternative_function_io_t&
   */
  static alternative_function_io_t& alternative_function_io()
  {
    if constexpr (hal::is_a_test()) {
      static alternative_function_io_t test_reg{};
      return test_reg;
    }
    return *reinterpret_cast<alternative_function_io_t*>(0x40010000);
  };

  static gpio_t& gpio(std::uint8_t p_port)
  {
    static gpio_t out_of_bounds_result{};
    if constexpr (hal::is_a_test()) {
      static std::array<gpio_t, 7> test_reg{};
      return test_reg[p_port - 'A'];
    }

    switch (p_port) {
      case 'A':
        return *reinterpret_cast<gpio_t*>(0x40010800);
      case 'B':
        return *reinterpret_cast<gpio_t*>(0x40010c00);
      case 'C':
        return *reinterpret_cast<gpio_t*>(0x40011000);
      case 'D':
        return *reinterpret_cast<gpio_t*>(0x40011400);
      case 'E':
        return *reinterpret_cast<gpio_t*>(0x40011800);
      case 'F':
        return *reinterpret_cast<gpio_t*>(0x40011c00);
      case 'G':
        return *reinterpret_cast<gpio_t*>(0x40012000);
      default:
        return out_of_bounds_result;
    }
  };

  /**
   * @brief Non-SWD JTAG pins to be used as GPIO
   *
   * The GPIO pins PB3, PB4, and PA15 are default initalized to be used for JTAG
   * purposes. If you are using SWD and want to use these pins as GPIO or as
   * other alternative functions, this function MUST be called.
   *
   */
  static void release_jtag_pins()
  {
    // Ensure that AFIO is powered on before attempting to access it
    power(peripheral::afio).on();
    // Set the JTAG Release code
    bit::modify(alternative_function_io().mapr)
      .insert<bit::mask::from<24, 26>()>(0b010U);
  }

  static constexpr pin_config_t push_pull_gpio_output = {
    .CNF1 = 0,
    .CNF0 = 0,
    .MODE = 0b11,  // Default to high speed 50 MHz
    .PxODR = 0b0,  // Default to 0 LOW Voltage
  };

  static constexpr pin_config_t open_drain_gpio_output = {
    .CNF1 = 0,
    .CNF0 = 1,
    .MODE = 0b11,  // Default to high speed 50 MHz
    .PxODR = 0b0,  // Default to 0 LOW Voltage
  };

  static constexpr pin_config_t push_pull_alternative_output = {
    .CNF1 = 1,
    .CNF0 = 0,
    .MODE = 0b11,  // Default to high speed 50 MHz
    .PxODR = 0b0,  // Default to 0 LOW Voltage
  };

  static constexpr pin_config_t open_drain_alternative_output = {
    .CNF1 = 1,
    .CNF0 = 1,
    .MODE = 0b11,  // Default to high speed 50 MHz
    .PxODR = 0b0,  // Default to 0 LOW Voltage
  };

  static constexpr pin_config_t input_analog = {
    .CNF1 = 0,
    .CNF0 = 0,
    .MODE = 0b00,
    .PxODR = 0b0,  // Don't care
  };

  static constexpr pin_config_t input_float = {
    .CNF1 = 0,
    .CNF0 = 1,
    .MODE = 0b00,
    .PxODR = 0b0,  // Don't care
  };

  static constexpr pin_config_t input_pull_down = {
    .CNF1 = 1,
    .CNF0 = 0,
    .MODE = 0b00,
    .PxODR = 0b0,  // Pull Down
  };

  static constexpr pin_config_t input_pull_up = {
    .CNF1 = 1,
    .CNF0 = 0,
    .MODE = 0b00,
    .PxODR = 0b1,  // Pull Up
  };

  /**
   * @brief
   *
   * @param port - must be a capitol letter from 'A' to 'G'
   * @param pin - must be between 0 to 15
   */
  constexpr pin(std::uint8_t p_port, std::uint8_t p_pin)
    : m_port(p_port)
    , m_pin(p_pin)
  {
  }

  /// Returns the a pointer the gpio port.
  gpio_t& port() const { return gpio(m_port); }

  /// Returns a bit mask indicating where the config bits are in the config
  /// registers.
  bit::mask mask() const
  {
    return {
      .position = static_cast<uint32_t>((m_pin * 4) % 32),
      .width = 4,
    };
  }

  /// Returns the configuration control register for the specific pin.
  /// Pins 0 - 7 are in CRL and Pins 8 - 15 are in CRH.
  volatile uint32_t& config_register() const
  {
    if (m_pin <= 7) {
      return port().crl;
    }
    return port().crh;
  }

  /// @return the 4 bits of this ports config.
  uint32_t config() const { return bit::extract(mask(), config_register()); }

  /// Set this ports 4 bits configuration.
  void config(pin_config_t p_config) const
  {

    constexpr auto cnf1 = bit::mask::from<3>();
    constexpr auto cnf0 = bit::mask::from<2>();
    constexpr auto mode = bit::mask::from<0, 1>();

    auto config = bit::value<std::uint32_t>(0)
                    .insert<cnf1>(p_config.CNF1)
                    .insert<cnf0>(p_config.CNF0)
                    .insert<mode>(p_config.MODE)
                    .get();

    config_register() =
      bit::modify(config_register()).insert(mask(), config).to<std::uint32_t>();
  }

private:
  std::uint8_t m_port;
  std::uint8_t m_pin;
};
}  // namespace hal::stm32::f10x::internal
