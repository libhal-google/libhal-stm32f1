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

#include <libhal-armcortex/interrupt.hpp>
#include <libhal-armcortex/systick_timer.hpp>
#include <libhal-stm32f1/clock.hpp>
#include <libhal-stm32f1/constants.hpp>
#include <libhal-stm32f1/output_pin.hpp>
#include <libhal-util/enum.hpp>
#include <libhal-util/steady_clock.hpp>

using namespace hal::literals;
using namespace std::literals;

volatile int poll_counter = 0;

hal::status application()
{
  hal::cortex_m::interrupt::initialize<hal::value(hal::stm32f1::irq::max)>();

  auto led = HAL_CHECK(hal::stm32f1::output_pin::get('C', 13));

  static hal::cortex_m::systick_timer timer(
    hal::stm32f1::frequency(hal::stm32f1::peripheral::cpu));

  HAL_CHECK(timer.schedule(
    [&led]() {
      bool level = led.level().value().state;
      // Invert the pin level with each call of this function.
      (void)led.level(!level);
    },
    500ms));

  while (true) {
    // Use a debugger to inspect this value to confirm its updates. This
    // approach aids in affirming that the timer interrupt is indeed being
    // executed.
    poll_counter = poll_counter + 1;
  }

  return hal::success();
}
