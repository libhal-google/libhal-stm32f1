#include <cinttypes>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal/error.hpp>

// Application function must be implemented by one of the compilation units
// (.cpp) files.
extern hal::status application();

int main()
{
  hal::cortex_m::initialize_data_section();

  auto is_finished = application();

  if (!is_finished) {
    hal::cortex_m::system_control::reset();
  } else {
    hal::halt();
  }

  return 0;
}

namespace boost {
void throw_exception(std::exception const& e)
{
  std::abort();
}
}  // namespace boost
