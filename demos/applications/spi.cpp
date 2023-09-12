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

#include <array>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-stm32f1/spi.hpp>
#include <libhal-stm32f1/uart.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/spi.hpp>
#include <libhal-util/steady_clock.hpp>

hal::status application()
{
  using namespace hal::literals;

  auto& clock = hal::stm32f1::clock::get();
  const auto cpu_frequency = clock.get_frequency(hal::stm32f1::peripheral::cpu);
  hal::cortex_m::dwt_counter steady_clock(cpu_frequency);

  // std::array<hal::byte, 32> uart_buffer{};
  // auto uart0 = HAL_CHECK(hal::stm32f1::uart::get(0,
  //                                              uart_buffer,
  //                                              hal::serial::settings{
  //                                                .baud_rate = 38400.0f,
  //                                              }));

  auto spi1 = HAL_CHECK(hal::stm32f1::spi::get(1));
  auto chip_select = HAL_CHECK(hal::stm32f1::output_pin::get(1, 10));
  auto chip_select_mirror = HAL_CHECK(hal::stm32f1::output_pin::get(1, 14));
  HAL_CHECK(chip_select.level(true));

  // hal::print(uart0, "Starting SPI Application...\n");

  while (true) {
    using namespace std::literals;
    std::array<hal::byte, 4> payload{ 0xDE, 0xAD, 0xBE, 0xEF };
    std::array<hal::byte, 8> buffer{};

    // hal::print(uart0, "Write operation\n");
    HAL_CHECK(hal::write(spi1, payload));
    hal::delay(steady_clock, 1s);

    // hal::print(uart0, "Read operation: [ ");
    HAL_CHECK(hal::read(spi1, buffer));
    // for (const auto& byte : buffer) {
    //   hal::print<32>(uart0, "0x%02X ", byte);
    // }
    // hal::print(uart0, "]\n");
    hal::delay(steady_clock, 1s);

    // hal::print(uart0, "Full-duplex transfer\n");
    HAL_CHECK(spi1.transfer(payload, buffer));
    hal::delay(steady_clock, 1s);

    // hal::print(uart0, "Half-duplex transfer\n");
    HAL_CHECK(hal::write_then_read(spi1, payload, buffer));
    hal::delay(steady_clock, 1s);

    {
      std::array read_manufacturer_id{ hal::byte{ 0x9F } };
      std::array<hal::byte, 4> id_data{};

      HAL_CHECK(chip_select.level(false));
      hal::delay(steady_clock, 250ns);
      HAL_CHECK(
        hal::write_then_read(spi1, read_manufacturer_id, id_data, 0xA5));
      HAL_CHECK(chip_select.level(true));

      // hal::print(uart0, "SPI Flash Memory ID info: ");
      // hal::print(uart0, "[ ");
      // for (const auto& byte : id_data) {
      //   hal::print<32>(uart0, "0x%02X ", byte);
      // }
      // hal::print(uart0, "]\n");
    }

    hal::delay(steady_clock, 1s);
  }

  return hal::success();
}
