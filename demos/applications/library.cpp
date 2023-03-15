#include <libhal-util/bit.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

hal::status application(hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& clock = *p_map.clock;
  [[maybe_unused]] auto& console = *p_map.console;

  while (true) {
    HAL_CHECK(hal::delay(clock, 500ms));
    HAL_CHECK(hal::delay(clock, 500ms));
  }

  return hal::success();
}
