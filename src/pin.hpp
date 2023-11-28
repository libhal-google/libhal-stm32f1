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
#include <cstdint>

#include <libhal/error.hpp>

namespace hal::stm32f1 {

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
 * @brief Pin select structure
 *
 */
struct pin_select_t
{
  /// @brief Port letter, must be a capitol letter from 'A' to 'G'
  std::uint8_t port;
  /// @brief Pin number, must be between 0 to 15
  std::uint8_t pin;
};

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
 * @brief Construct pin manipulation object
 *
 * @param p_pin_select - the pin to configure
 * @param p_config - Configuration to set the pin to
 */
void configure_pin(pin_select_t p_pin_select, pin_config_t p_config);

/**
 * @brief Returns the gpio register based on the port
 *
 * @param p_port - port letter, must be from 'A' to 'G'
 * @return gpio_t& - gpio register map
 */
gpio_t& gpio(std::uint8_t p_port);

inline alternative_function_io_t* alternative_function_io =
  reinterpret_cast<alternative_function_io_t*>(0x4001'0000);
inline gpio_t* gpio_a_reg = reinterpret_cast<gpio_t*>(0x4001'0800);
inline gpio_t* gpio_b_reg = reinterpret_cast<gpio_t*>(0x4001'0c00);
inline gpio_t* gpio_c_reg = reinterpret_cast<gpio_t*>(0x4001'1000);
inline gpio_t* gpio_d_reg = reinterpret_cast<gpio_t*>(0x4001'1400);
inline gpio_t* gpio_e_reg = reinterpret_cast<gpio_t*>(0x4001'1800);
inline gpio_t* gpio_f_reg = reinterpret_cast<gpio_t*>(0x4001'1c00);
inline gpio_t* gpio_g_reg = reinterpret_cast<gpio_t*>(0x4001'2000);
}  // namespace hal::stm32f1
