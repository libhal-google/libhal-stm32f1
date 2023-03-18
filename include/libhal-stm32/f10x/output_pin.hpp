#pragma once

#include <cstdint>

#include <libhal/output_pin.hpp>

#include "internal/pin.hpp"
#include "internal/platform_check.hpp"
#include "power.hpp"

namespace hal::stm32::f10x {
/**
 * @brief Output pin implementation for the stm32::f10x
 *
 */
class output_pin : public hal::output_pin
{
public:
  /**
   * @brief Get the output pin object
   *
   * @tparam port - selects pin port to use
   * @tparam pin - selects which pin within the port to use
   * @param p_settings - initial pin settings
   * @return result<output_pin&> - reference to the statically allocated output
   * pin
   */
  template<std::uint8_t port, std::uint8_t pin>
  static result<output_pin&> get(output_pin::settings p_settings = {})
  {
    compile_time_platform_check();
    internal::check_gpio_bounds_at_compile<port, pin>();
    static output_pin gpio(port, pin);

    // Ensure that AFIO is powered on before attempting to access it
    power(peripheral::afio).on();

    if constexpr (port == 'A') {
      power(peripheral::gpio_a).on();
    } else if constexpr (port == 'B') {
      power(peripheral::gpio_b).on();
    } else if constexpr (port == 'C') {
      power(peripheral::gpio_c).on();
    } else if constexpr (port == 'D') {
      power(peripheral::gpio_d).on();
    } else if constexpr (port == 'E') {
      power(peripheral::gpio_e).on();
    }

    // Ignore result as this function is infallible
    (void)gpio.driver_configure(p_settings);
    return gpio;
  }

private:
  output_pin(std::uint8_t p_port, std::uint8_t p_pin)
    : m_port(p_port)
    , m_pin(p_pin)
  {
  }

  status driver_configure(const settings& p_settings) override;
  result<set_level_t> driver_level(bool p_high) override;
  result<level_t> driver_level() override;

  std::uint8_t m_port{};
  std::uint8_t m_pin{};
};

inline status output_pin::driver_configure(const settings& p_settings)
{
  if (!p_settings.open_drain) {
    internal::pin(m_port, m_pin).config(internal::pin::push_pull_gpio_output);
  } else if (p_settings.open_drain) {
    internal::pin(m_port, m_pin).config(internal::pin::open_drain_gpio_output);
  }

  return hal::success();
}

inline result<output_pin::set_level_t> output_pin::driver_level(bool p_high)
{
  if (p_high) {
    // The first 16 bits of the register set the output state
    internal::pin(m_port, m_pin).port().bsrr = 1 << m_pin;
  } else {
    // The last 16 bits of the register reset the output state
    internal::pin(m_port, m_pin).port().bsrr = 1 << (16 + m_pin);
  }

  return set_level_t{};
}

inline result<output_pin::level_t> output_pin::driver_level()
{
  auto pin_value = bit::extract(bit::mask::from(m_pin),
                                internal::pin(m_port, m_pin).port().idr);

  return level_t{ .state = static_cast<bool>(pin_value) };
}
}  // namespace hal::stm32::f10x
