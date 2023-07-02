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

#include <libhal-stm32f1/output_pin.hpp>

#include <cstdint>

#include <libhal-util/bit.hpp>

#include "pin.hpp"
#include "power.hpp"

namespace hal::stm32f1 {
result<output_pin> output_pin::get(std::uint8_t p_port,
                                   std::uint8_t p_pin,
                                   output_pin::settings p_settings)
{
  output_pin gpio(p_port, p_pin);

  // Ensure that AFIO is powered on before attempting to access it
  power(peripheral::afio).on();

  if (p_port == 'A') {
    power(peripheral::gpio_a).on();
  } else if (p_port == 'B') {
    power(peripheral::gpio_b).on();
  } else if (p_port == 'C') {
    power(peripheral::gpio_c).on();
  } else if (p_port == 'D') {
    power(peripheral::gpio_d).on();
  } else if (p_port == 'E') {
    power(peripheral::gpio_e).on();
  } else {
    return hal::new_error(std::errc::invalid_argument);
  }

  // Ignore result as this function is infallible
  (void)gpio.driver_configure(p_settings);
  return gpio;
}

output_pin::output_pin(std::uint8_t p_port, std::uint8_t p_pin)
  : m_port(p_port)
  , m_pin(p_pin)
{
}

status output_pin::driver_configure(const settings& p_settings)
{
  if (!p_settings.open_drain) {
    configure_pin(m_port, m_pin, push_pull_gpio_output);
  } else if (p_settings.open_drain) {
    configure_pin(m_port, m_pin, open_drain_gpio_output);
  }

  return hal::success();
}

result<output_pin::set_level_t> output_pin::driver_level(bool p_high)
{
  if (p_high) {
    // The first 16 bits of the register set the output state
    gpio(m_port).bsrr = 1 << m_pin;
  } else {
    // The last 16 bits of the register reset the output state
    gpio(m_port).bsrr = 1 << (16 + m_pin);
  }

  return set_level_t{};
}

result<output_pin::level_t> output_pin::driver_level()
{
  auto pin_value = bit::extract(bit::mask::from(m_pin), gpio(m_port).idr);

  return level_t{ .state = static_cast<bool>(pin_value) };
}
}  // namespace hal::stm32f1
