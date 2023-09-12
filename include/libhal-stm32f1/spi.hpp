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

#include <atomic>
#include <cstdint>
#include <span>

#include <libhal/spi.hpp>

#include "constants.hpp"
#include "pin.hpp"

namespace hal::stm32f1 {
class spi : public hal::spi
{
public:
  /// Information used to configure the spi bus
  struct bus_info
  {
    /// peripheral id used to power on the spi peripheral at creation
    peripheral peripheral_id;
    /// spi clock port
    char clk_port;
    /// spi MOSI port
    char mosi_port;
    /// spi MISO port
    char miso_port;
    /// spi clock pin
    std::uint8_t clk_pin;
    /// spi MOSI pin
    std::uint8_t mosi_pin;
    /// spi MISO pin
    std::uint8_t miso_pin;
  };

  static result<spi> get(std::uint8_t p_bus,
                         const spi::settings& p_settings = {});

  spi(spi& p_other) = delete;
  spi& operator=(spi& p_other) = delete;
  spi(spi&& p_other);
  spi& operator=(spi&& p_other);
  ~spi();

private:
  spi(bus_info p_bus);

  status driver_configure(const settings& p_settings) override;
  result<transfer_t> driver_transfer(std::span<const hal::byte> p_data_out,
                                     std::span<hal::byte> p_data_in,
                                     hal::byte p_filler) override;

  bus_info m_info;
  bool m_moved = false;
};
}  // namespace hal::stm32f1
