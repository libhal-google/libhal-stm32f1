#include <libstm32f1xx/platform/stm32f10x.h>

#include <libarmcortex/peripherals/interrupt.hpp>
#include <libarmcortex/peripherals/system_timer.hpp>
#include <libarmcortex/platform/exceptions.hpp>
#include <libcore/platform/newlib.hpp>
#include <libcore/utility/time/time.hpp>
#include <libstm32f1xx/peripherals/gpio.hpp>
#include <libstm32f1xx/peripherals/system_controller.hpp>

// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
[[gnu::section(".vector")]]
// NOLINTNEXTLINE(readability-identifier-naming)
const sjsu::InterruptVectorAddress kInterruptVectorTable[16] = {
  // Core Level - CM3/4
  &StackTop,               // 0, The initial stack pointer
  ArmResetHandler,         // 1, The reset handler
  ArmNMIHandler,           // 2, The NMI handler
  ArmHardFaultHandler,     // 3, The hard fault handler
  ArmMemoryManageHandler,  // 4, The MPU fault handler
  ArmBusFaultHandler,      // 5, The bus fault handler
  ArmUsageFaultHandler,    // 6, The usage fault handler
  nullptr,                 // 7, Reserved
  nullptr,                 // 8, Reserved
  nullptr,                 // 9, Reserved
  nullptr,                 // 10, Reserved
  nullptr,                 // 11, SVCall handler
  nullptr,                 // 12, Debug monitor handler
  nullptr,                 // 13, Reserved
  nullptr,                 // 14, PendSV Handler
  ArmSystemTickHandler,    // 15, The SysTick handler
  // Chip Level - stm32f10x
  // 0, Window WatchDog
  // 1, PVD through EXTI Line detection
  // 2, Tamper
  // 3, RTC
  // 4, FLASH
  // 5, RCC
  // 6, EXTI Line0
  // 7, EXTI Line1
  // 8, EXTI Line2
  // 9, EXTI Line3
  // 10, EXTI Line4
  // 11, DMA1 Channel 1
  // 12, DMA1 Channel 2
  // 13, DMA1 Channel 3
  // 14, DMA1 Channel 4
  // 15, DMA1 Channel 5
  // 16, DMA1 Channel 6
  // 17, DMA1 Channel 7
  // All of the exceptions beyond this point are different depending on the
  // variant of stm32f10x you are using
  // 18
  // 19
  // 20
  // 21
  // 22
  // 23
  // 24
  // 25
  // 26
  // 27
  // 28
  // 29
  // 30
  // 31
  // 32
  // 33
  // 34
  // 35
  // 36
  // 37
  // 38
  // 39
  // 40
  // 41
  // 42
  // 43
  // 44
  // 45
  // 46
  // 47
  // 48
  // 49
  // 50
  // 51
  // 52
  // 53
  // 54
  // 55
  // 56
  // 57
  // 58
  // 59
  // 60
  // 61
  // 62
  // 63
  // 64
  // 65
  // 66
  // 67
};

// Private namespace to make sure that these do not conflict with other globals
namespace
{
// Default initialized clock configuration object for use in the system
// controller.
sjsu::stm32f10x::SystemController::ClockConfiguration clock_configuration;

// Create stm32f10x system controller to be used by low level initialization.
sjsu::stm32f10x::SystemController system_controller(clock_configuration);

// System timer is used to count milliseconds of time and to run the RTOS
// scheduler.
sjsu::cortex::SystemTimer system_timer(sjsu::stm32f10x::kSystemTimer);

// Cortex NVIC interrupt controller used to setup interrupt service routines
sjsu::cortex::InterruptController<67, 4> interrupt_controller(
    kInterruptVectorTable);
}  // namespace

namespace sjsu
{
void InitializePlatform()
{
  sjsu::AddNewlibSymbols();
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
}  // namespace sjsu
