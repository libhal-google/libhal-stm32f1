#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include "../../hardware_map.hpp"

hal::result<hardware_map> initialize_target()
{
  using namespace hal::literals;
  hal::cortex_m::initialize_data_section();
  static hal::cortex_m::dwt_counter counter(12.0_MHz);

  return hardware_map{
    .console = nullptr,
    .clock = &counter,
    .reset = []() { hal::cortex_m::system_control::reset(); },
  };
}
