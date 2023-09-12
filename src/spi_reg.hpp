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

#include <cstdint>

#include <libhal-util/bit.hpp>

namespace hal::stm32f1 {
struct spi_reg_t
{
  /*!< Offset: 0x000 Control Register 0 (R/W) */
  volatile uint32_t cr1;
  /*!< Offset: 0x004 Control Register 1 (R/W) */
  volatile uint32_t cr2;
  /*!< Offset: 0x008 Status Register (R/ ) */
  volatile uint32_t sr;
  /*!< Offset: 0x00C Data Register (R/W) */
  const volatile uint32_t dr;
};

// SPI Control Register 1 (SPI_CR1)
struct control_register1
{
  // CPHA: Clock phase
  //   0: The first clock transition is the first data capture edge
  //   1: The second clock transition is the first data capture edge
  static constexpr auto phase_bit = bit_mask::from<0>();

  // CPOL: Clock polarity
  //   0: CK to 0 when idle
  //   1: CK to 1 when idle
  static constexpr auto polarity_bit = bit_mask::from<1>();

  // MSTR: Master selection
  //   0: Slave configuration
  //   1: Master configuration
  static constexpr auto master_select_bit = bit_mask::from<2>();

  // BR[2:0] Baud rate control
  //   000: fPCLK/2
  //   001: fPCLK/4
  //   010: fPCLK/8
  //   011: fPCLK/16
  //   100: fPCLK/32
  //   101: fPCLK/64
  //   110: fPCLK/128
  //   111: fPCLK/256
  static constexpr auto baud_rate_bit = bit_mask::from<3, 5>();

  // SPE: SPI enable
  //   0: Peripheral disabled
  //   1: Peripheral enabled
  static constexpr auto spi_enable_bit = bit_mask::from<6>();

  // LSBFIRST: Frame format
  //   0: MSB transmitted first
  //   1: LSB transmitted first
  static constexpr auto lsb_first_bit = bit_mask::from<7>();

  // SSI: Internal slave select
  //   This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the
  //   NSS pin and the IO value of the NSS pin is ignored
  static constexpr auto ssi_bit = bit_mask::from<8>();

  // SSM: Software Slave Management
  // When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
  //   0: Software slave management disabled
  //   1: Software slave management enabled
  static constexpr auto ssm_bit = bit_mask::from<9>();

  // RXONLY: Receive Only
  // This bit combined with the BIDImode bit selects the direction of transfer in 2-line
  // unidirectional mode. This bit is also useful in a multislave system in which this particular
  // slave is not accessed, the output from the accessed slave is not corrupted.
  //   0: Full duplex (Transmit and receive)
  //   1: Output disabled (Receive-only mode)
  static constexpr auto rx_only_bit = bit_mask::from<10>();

  // DFF: Data frame format
  //   0: 8-bit data frame format is selected for transmission/reception
  //   1: 16-bit data frame format is selected for transmission/reception
  static constexpr auto data_frame_format_bit = bit_mask::from<11>();

  // CRC transfer next
  //   0: Data phase (no CRC phase)
  //   1: Next transfer is CRC (CRC phase)
  static constexpr auto crc_next_bit = bit_mask::from<12>();

  // CRCEN: Hardware CRC calculation enable
  //   0: CRC calculation disabled
  //   1: CRC calculation enabled
  static constexpr auto crc_enable_bit = bit_mask::from<13>();

  // BIDIOE: This bit combined with the BIDImode bit selects the direction of transfer in bidirectional mode
  //   0: Output disabled (receive-only mode)
  //   1: Output enabled (transmit-only mode)
  static constexpr auto bidioe_bit = bit_mask::from<14>();

  // BIDIMODE: Bidirectional data mode enable
  //   0: 2-line unidirectional data mode selected
  //   1: 1-line bidirectional data mode selected
  static constexpr auto bidimode_bit = bit_mask::from<15>();
}

// SPI Control Register 2 (SPI_CR2)
struct control_register2
{
  // When this bit is set, the DMA request is made whenever the RXNE flag is set.
  //   0: Rx buffer DMA disabled
  //   1: Rx buffer DMA enabled
  static constexpr auto rx_dma_enable_bit = bit_mask::from<0>();

  // When this bit is set, the DMA request is made whenever the TXE flag is set.
  //   0: Tx buffer DMA disabled
  //   1: Tx buffer DMA enabled
  static constexpr auto tx_dma_enable_bit = bit_mask::from<1>();

  // SS output enable
  //   0: SS output is disabled in master mode and the cell can work in multimaster configuration
  //   1: SS output is enabled in master mode and when the cell is enabled. The cell cannot work
  //   in a multimaster environment.
  static constexpr auto ss_out_enable_bit = bit_mask::from<2>();

  // This bit controls the generation of an interrupt when an error condition occurs (CRCERR,
  // OVR, MODF in SPI mode and UDR, OVR in I2S mode).
  //   0: Error interrupt is masked
  //   1: Error interrupt is enabled
  static constexpr auto error_interrupt_enable_bit = bit_mask::from<5>();

  // RX buffer not empty interrupt enable
  //   0: RXNE interrupt masked
  //   1: RXNE interrupt not masked. Used to generate an interrupt request when the RXNE flag is
  //   set.
  static constexpr auto rx_not_empty_interrupt_enable_bit = bit_mask::from<6>();

  // TXEIE: Tx buffer empty interrupt enable
  //   0: TXE interrupt masked
  //   1: TXE interrupt not masked. Used to generate an interrupt request when the TXE flag is set.
  static constexpr auto tx_empty_interrupt_enable_bit = bit_mask::from<7>();
}

/// SPI Status Register (SPI_SR)
struct status_register
{
  // Receive buffer not empty
  //   0: Rx buffer empty
  //   1: Rx buffer not empty
  static constexpr auto rxne_bit = bit_mask::from<0>();

  // Transmit buffer empty
  //   0: Tx buffer not empty
  //   1: Tx buffer empty
  static constexpr auto txe_bit = bit_mask::from<1>();
};

inline spi_reg_t* spi_reg1 = reinterpret_cast<spi_reg_t*>(0x40013000);
inline spi_reg_t* spi_reg2 = reinterpret_cast<spi_reg_t*>(0x40003800);
}  // namespace hal::stm32f1
