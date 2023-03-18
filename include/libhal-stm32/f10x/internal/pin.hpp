#pragma once

#include <array>
#include <cstdint>

#include <libhal-util/bit.hpp>
#include <libhal/config.hpp>

#include "../power.hpp"

namespace stm32::f10x::internal {
class pin
{
public:
  /**
   * @brief Alternate Function I/O Register Map
   */
  struct alternative_function_io_t
  {
    volatile std::uint32_t EVCR;
    volatile std::uint32_t MAPR;
    volatile std::array<std::uint32_t, 4> EXTICR;
    std::uint32_t RESERVED0;
    volatile std::uint32_t MAPR2;
  };

  /**
   * @brief GPIO register map
   *
   */
  struct gpio_t
  {
    volatile std::uint32_t CRL;
    volatile std::uint32_t CRH;
    volatile std::uint32_t IDR;
    volatile std::uint32_t ODR;
    volatile std::uint32_t BSRR;
    volatile std::uint32_t BRR;
    volatile std::uint32_t LCKR;
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
    bit::modify(alternative_function_io().MAPR)
      .insert<bit::mask::from<24, 26>()>(0b010);
  }

  /**
   * @brief
   *
   * @param port - must be a capitol letter from 'A' to 'I'
   * @param pin - must be between 0 to 15
   */
  constexpr pin(std::uint8_t p_port, std::uint8_t p_pin)
    : m_port(p_port)
    , m_pin(p_pin)
  {
  }

private:
  std::uint8_t m_port;
  std::uint8_t m_pin;
};
}  // namespace stm32::f10x::internal
