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

#include <libhal-util/bit.hpp>
#include <libhal-util/enum.hpp>

#include <libhal-stm32f1/constants.hpp>

#include "power.hpp"
#include "rcc_reg.hpp"

namespace hal::stm32f1 {
namespace {
volatile uint32_t* enable(int p_bus_index)
{
  switch (p_bus_index) {
    case 0:
      return &rcc->ahbenr;
    case 1:
      return &rcc->apb1enr;
    case 2:
      return &rcc->apb2enr;
    default:
      return nullptr;
  }
}
}  // namespace

power::power(peripheral p_peripheral)
{
  auto peripheral_value = hal::value(p_peripheral);
  auto bus_number = peripheral_value / bus_id_offset;

  m_bit_position = static_cast<std::uint8_t>(peripheral_value % bus_id_offset);
  m_enable_register = enable(bus_number);
}

void power::on()
{
  if (m_enable_register) {
    hal::bit_modify(*m_enable_register).set(bit_mask::from(m_bit_position));
  }
}

bool power::is_on()
{
  if (m_enable_register) {
    return hal::bit_extract(bit_mask::from(m_bit_position), *m_enable_register);
  }
  return true;
}

void power::off()
{
  if (m_enable_register) {
    hal::bit_modify(*m_enable_register).clear(bit_mask::from(m_bit_position));
  }
}
}  // namespace hal::stm32f1