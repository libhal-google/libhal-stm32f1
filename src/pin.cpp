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

#include <libhal-stm32f1/pin.hpp>

#include <array>
#include <cstdint>

#include <libhal-util/bit.hpp>

#include "pin.hpp"
#include "power.hpp"

namespace hal::stm32f1 {
namespace {
/// Returns a bit mask indicating where the config bits are in the config
/// registers.
bit::mask mask(std::uint8_t p_pin)
{
  return {
    .position = static_cast<uint32_t>((p_pin * 4) % 32),
    .width = 4,
  };
}

/// Returns the configuration control register for the specific pin.
/// Pins 0 - 7 are in CRL and Pins 8 - 15 are in CRH.
volatile uint32_t& config_register(char p_port, std::uint8_t p_pin)
{
  if (p_pin <= 7) {
    return gpio(p_port).crl;
  }
  return gpio(p_port).crh;
}

/// @return the 4 bits of this ports config.
uint32_t config(char p_port, std::uint8_t p_pin)
{
  return bit::extract(mask(p_pin), config_register(p_port, p_pin));
}
}  // namespace

gpio_t& gpio(char p_port)
{
  static gpio_t out_of_bounds_result{};

  switch (p_port) {
    case 'A':
      return *gpio_a;
    case 'B':
      return *gpio_b;
    case 'C':
      return *gpio_c;
    case 'D':
      return *gpio_d;
    case 'E':
      return *gpio_e;
    case 'F':
      return *gpio_f;
    case 'G':
      return *gpio_g;
    default:
      return out_of_bounds_result;
  }
};

void configure_pin(char p_port, std::uint8_t p_pin, pin_config_t p_config)
{
  constexpr auto cnf1 = bit::mask::from<3>();
  constexpr auto cnf0 = bit::mask::from<2>();
  constexpr auto mode = bit::mask::from<0, 1>();

  auto config = bit::value<std::uint32_t>(0)
                  .insert<cnf1>(p_config.CNF1)
                  .insert<cnf0>(p_config.CNF0)
                  .insert<mode>(p_config.MODE)
                  .get();

  config_register(p_port, p_pin) = bit::modify(config_register(p_port, p_pin))
                                     .insert(mask(p_pin), config)
                                     .to<std::uint32_t>();
}

void release_jtag_pins()
{
  // Ensure that AFIO is powered on before attempting to access it
  power(peripheral::afio).on();
  // Set the JTAG Release code
  bit::modify(alternative_function_io->mapr)
    .insert<bit::mask::from<24, 26>()>(0b010U);
}
}  // namespace hal::stm32f1
