#pragma once

#include <libhal-util/bit.hpp>
#include <libhal-util/enum.hpp>
#include <libhal/config.hpp>

#include "constants.hpp"
#include "internal/reset_and_clock_control.hpp"

namespace hal::stm32f1 {

/**
 * @brief Power control for lpc40xx peripherals
 *
 */
class power
{
public:
  /**
   * @brief Construct a new power control object
   *
   * @param p_peripheral - id of the peripheral to configure
   */
  power(peripheral p_peripheral)
  {
    auto peripheral_value = hal::value(p_peripheral);
    auto bus_number = peripheral_value / bus_id_offset;

    m_bit_position =
      static_cast<std::uint8_t>(peripheral_value % bus_id_offset);
    m_enable_register = enable(bus_number);
  }

  /**
   * @brief Power on the peripheral
   *
   */
  void on()
  {
    if (m_enable_register) {
      hal::bit::modify(*m_enable_register).set(bit::mask::from(m_bit_position));
    }
  }

  /**
   * @brief Check if the peripheral is powered on
   *
   * @return true - peripheral is on
   * @return false - peripheral is off
   */
  [[nodiscard]] bool is_on()
  {
    if (m_enable_register) {
      return hal::bit::extract(bit::mask::from(m_bit_position),
                               *m_enable_register);
    }
    return true;
  }

  /**
   * @brief Power off peripheral
   *
   */
  void off()
  {
    if (m_enable_register) {
      hal::bit::modify(*m_enable_register)
        .clear(bit::mask::from(m_bit_position));
    }
  }

private:
  static volatile uint32_t* enable(int p_bus_index)
  {
    switch (p_bus_index) {
      case 0:
        return &internal::rcc().ahbenr;
      case 1:
        return &internal::rcc().apb1enr;
      case 2:
        return &internal::rcc().apb2enr;
      default:
        return nullptr;
    }
    return nullptr;
  }

  volatile std::uint32_t* m_enable_register = nullptr;
  std::uint8_t m_bit_position = 0;
};
}  // namespace hal::stm32f1