#pragma once

#include <libstm32f10x/platform/stm32f10x.h>

#include <libarmcortex/peripherals/interrupt.hpp>
#include <libarmcortex/peripherals/system_timer.hpp>
#include <libcore/platform/syscall.hpp>
#include <libcore/utility/time/time.hpp>
#include <libstm32f10x/peripherals/gpio.hpp>
#include <libstm32f10x/peripherals/system_controller.hpp>

namespace sjsu::stm32f10x
{
void InitializePlatform()
{
  // Default initialized clock configuration object for use in the system
  // controller.
  static sjsu::stm32f10x::SystemController::ClockConfiguration
      clock_configuration;

  // Create stm32f10x system controller to be used by low level initialization.
  static sjsu::stm32f10x::SystemController system_controller(
      clock_configuration);

  // System timer is used to count milliseconds of time and to run the RTOS
  // scheduler.
  static sjsu::cortex::SystemTimer system_timer(sjsu::stm32f10x::kSystemTimer);

  // Cortex NVIC interrupt controller used to setup interrupt service routines
  static sjsu::cortex::InterruptController<67, 4> interrupt_controller;

  sjsu::AddSysCallSymbols();

  // Set the platform's interrupt controller.
  // This will be used by other libraries to enable and disable interrupts.
  sjsu::InterruptController::SetPlatformController(&interrupt_controller);
  sjsu::SystemController::SetPlatformController(&system_controller);

  system_controller.Initialize();
  interrupt_controller.Initialize();
  system_timer.Initialize();

  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);

  // The GPIO pins PB3, PB4, and PA15 are default initalized to be used for
  // JTAG purposes. They are not needed for SWD and are commonly used for other
  // functions, so releasing them from their usage as JTAG pins will enable
  // more these pins for use in application software. They are so commonly used
  // that, it makes sense to simply release them at platform initialization.
  sjsu::stm32f10x::Gpio::ReleaseJTAGPins();
}
}  // namespace sjsu::stm32f10x
