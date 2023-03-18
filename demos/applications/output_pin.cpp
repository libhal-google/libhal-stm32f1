
#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-stm32/f10x/clock.hpp>
#include <libhal-stm32/f10x/output_pin.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

hal::status application()
{
  using namespace hal::literals;
  hal::cortex_m::dwt_counter clock(hal::stm32::f10x::clock::get().get_frequency(
    hal::stm32::f10x::peripheral::cpu));

  auto& led = HAL_CHECK((hal::stm32::f10x::output_pin::get<'C', 13>()));

  while (true) {
    using namespace std::chrono_literals;
    HAL_CHECK(led.level(false));
    HAL_CHECK(hal::delay(clock, 200ms));
    HAL_CHECK(led.level(true));
    HAL_CHECK(hal::delay(clock, 200ms));
  }
}
