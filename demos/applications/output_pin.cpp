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


#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

hal::status application()
{
  using namespace hal::literals;
  hal::cortex_m::dwt_counter clock(
    hal::stm32f1::clock::get().get_frequency(hal::stm32f1::peripheral::cpu));

  auto& led = HAL_CHECK((hal::stm32f1::output_pin::get<'C', 13>()));

  while (true) {
    using namespace std::chrono_literals;
    HAL_CHECK(led.level(false));
    HAL_CHECK(hal::delay(clock, 200ms));
    HAL_CHECK(led.level(true));
    HAL_CHECK(hal::delay(clock, 200ms));
  }
}
