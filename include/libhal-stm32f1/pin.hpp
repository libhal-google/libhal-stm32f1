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

namespace hal::stm32f1 {
/**
 * @brief Make JTAG pins not associated with SWD available as IO
 *
 * The GPIO pins PB3, PB4, and PA15 are default initalized to be used for JTAG
 * purposes. If you are using SWD and want to use these pins as GPIO or as
 * other alternative functions, this function MUST be called.
 *
 */
void release_jtag_pins();
}  // namespace hal::stm32f1
