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

#include <cstdint>

#include <libhal-stm32f1/spi.hpp>

#include <libhal-armcortex/interrupt.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/power.hpp>
#include <libhal-util/bit.hpp>
#include <libhal-util/spi.hpp>
#include <libhal-util/static_callable.hpp>

#include "spi_reg.hpp"

namespace hal::stm32f1 {
namespace {
inline spi_reg_t* get_spi_reg(peripheral p_id)
{
  switch (p_id) {
    case peripheral::spi1:
      return spi_reg1;
    case peripheral::spi2:
      return spi_reg2;
    default:
      return spi_reg1;
  }
}

inline bool busy(spi_reg_t* p_reg)
{
  return bit_extract<status_register::rxne_bit>(p_reg->sr);
}
}  // namespace

result<spi> spi::get(std::uint8_t p_bus_number, const spi::settings& p_settings)
{
  spi::bus_info bus_info{};

  // UM10562: Chapter 7: LPC408x/407x I/O configuration page 13
  if (p_bus_number == 1) {
    bus_info = {
      .peripheral_id = peripheral::spi1,
      .clk_port = 'A',
      .clk_pin = 5,
      .miso_port = 'A',
      .miso_pin = 6,
      .mosi_port = 'A',
      .mosi_pin = 7
    };

  } else if (p_bus_number == 2) {
    bus_info = {
      .peripheral_id = peripheral::spi2,
      .clk_port = 'B',
      .clk_pin = 13,
      .miso_port = 'B',
      .miso_pin = 14,
      .mosi_port = 'B',
      .mosi_pin = 15
    };

  } else {
    // "Supported spi busses are 0, 1, and 2!";
    return hal::new_error(std::errc::invalid_argument);
  }

  spi spi_bus(bus_info);
  HAL_CHECK(spi_bus.driver_configure(p_settings));

  return spi_bus;
}

spi::spi(spi&& p_other)
{
  p_other.m_moved = true;
}

spi& spi::operator=(spi&& p_other)
{
  p_other.m_moved = true;
  return *this;
}

spi::~spi()
{
  if (!m_moved) {
    power(m_info.peripheral_id).off();
  }
}

spi::spi(bus_info p_bus)
  : m_info(p_bus)
{
}


/*
RM0008 25.3.3 (pg 707)
Configure SPI in Master Mode:
*/
status spi::driver_configure(const settings& p_settings)
{
  auto* reg = get_spi_reg(m_info.peripheral_id);

  // Power up peripheral
  power(m_info.peripheral_id).on();

  // 1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
  constexpr uint8_t baud_rate_code = 0b000; // TODO
  bit_modify(reg->cr1).insert<control_register1::baud_rate_bit>(baud_rate_code);

  // 2. Select the CPOL and CPHA bits to define one of the four relationships between the
  //    data transfer and the serial clock (see Figure 240).
  // 3. Set the DFF bit to define 8- or 16-bit data frame format
  bit_modify(reg->cr1)
    .insert<control_register1::polarity_bit>(p_settings.clock_idles_high)
    .insert<control_register1::phase_bit>(p_settings.data_valid_on_trailing_edge)
    .set<control_register1::data_frame_format_bit>();

  // 5. If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
  //    high-level signal during the complete byte transmit sequence. In NSS software mode,
  //    set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
  //    mode, the SSOE bit only should be set.
  bit_modify(reg->cr1)
    .set(control_register1::ssm_bit)()
    .set(control_register1::ssi_bit)();

  // Initialize SPI pins
  configure_pin(m_info.clk_port, m_info.clk_pin, pin::push_pull_gpio_output);
  configure_pin(m_info.miso_port, m_info.miso_pin, pin::push_pull_gpio_output);
  configure_pin(m_info.mosi_port, m_info.mosi_pin, pin::push_pull_gpio_output);

  // 6. Set MSTR and SPE bits (they remain set only if the NSS pin is connected
  //    to a high-level signal).
  bit_modify(reg->cr1)
    .set(control_register1::master_select_bit)()
    .set(control_register1::spi_enable_bit)();

  return hal::success();
}

/*
Handling data transmission and reception:
  The TXE flag (Tx buffer empty) is set when the data are transferred from the Tx buffer to the
  shift register. It indicates that the internal Tx buffer is ready to be loaded with the next data.
  An interrupt can be generated if the TXEIE bit in the SPI_CR2 register is set. Clearing the
  TXE bit is performed by writing to the SPI_DR register.

  Note: The software must ensure that the TXE flag is set to 1 before attempting to write to the Tx
  buffer. Otherwise, it overwrites the data previously written to the Tx buffer.

In full-duplex (BIDIMODE=0 and RXONLY=0)
  1. Enable the SPI by setting the SPE bit to 1.
  2. Write the first data item to be transmitted into the SPI_DR register (this clears the TXE
     flag).
  3. Wait until TXE=1 and write the second data item to be transmitted. Then wait until
     RXNE=1 and read the SPI_DR to get the first received data item (this clears the RXNE
     bit). Repeat this operation for each data item to be transmitted/received until the n–1
     received data.
  4. Wait until RXNE=1 and read the last received data.
  5. Wait until TXE=1 and then wait until BSY=0 before disabling the SPI.
*/
result<spi::transfer_t> spi::driver_transfer(
  std::span<const hal::byte> p_data_out,
  std::span<hal::byte> p_data_in,
  hal::byte p_filler)
{
  auto* reg = get_spi_reg(m_info.peripheral_id);
  size_t max_length = std::max(p_data_in.size(), p_data_out.size());

  for (size_t index = 0; index < max_length; index++) {
    hal::byte byte = 0;

    if (index < p_data_out.size()) {
      byte = p_data_out[index];
    } else {
      byte = p_filler;
    }

    reg->dr = byte;

    while (busy(reg)) {
      continue;
    }

    byte = static_cast<uint8_t>(reg->dr);
    if (index < p_data_in.size()) {
      p_data_in[index] = byte;
    }
  }

  return spi::transfer_t{};
}
}  // namespace hal::stm32f1
