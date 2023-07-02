#pragma once

#include <array>
#include <cstdint>

namespace hal::stm32f1 {
struct flash_t
{
  volatile std::uint32_t acr;
  volatile std::uint32_t keyr;
  volatile std::uint32_t optkeyr;
  volatile std::uint32_t sr;
  volatile std::uint32_t cr;
  volatile std::uint32_t ar;
  volatile std::uint32_t reserved;
  volatile std::uint32_t obr;
  volatile std::uint32_t wrpr;
  std::array<uint32_t, 8> reserved1;
  volatile std::uint32_t keyr2;
  uint32_t reserved2;
  volatile std::uint32_t sr2;
  volatile std::uint32_t cr2;
  volatile std::uint32_t ar2;
};

/// Pointer to the flash control register
flash_t* flash = reinterpret_cast<flash_t*>(0x4002'2000);
}  // namespace hal::stm32f1
