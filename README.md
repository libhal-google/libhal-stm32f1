# libhal-stm32f1

[![✅ Checks](https://github.com/libhal/libhal-stm32f1/actions/workflows/ci.yml/badge.svg)](https://github.com/libhal/libhal-stm32f1/actions/workflows/ci.yml)
[![Coverage](https://libhal.github.io/libhal-stm32f1/coverage/coverage.svg)](https://libhal.github.io/libhal-stm32f1/coverage/)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/b084e6d5962d49a9afcb275d62cd6586)](https://www.codacy.com/gh/libhal/libhal-stm32f1/dashboard?utm_source=github.com&utm_medium=referral&utm_content=libhal/libhal-stm32f1&utm_campaign=Badge_Grade)
[![GitHub stars](https://img.shields.io/github/stars/libhal/libhal-stm32f1.svg)](https://github.com/libhal/libhal-stm32f1/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/libhal/libhal-stm32f1.svg)](https://github.com/libhal/libhal-stm32f1/network)
[![GitHub issues](https://img.shields.io/github/issues/libhal-stm32f1/libhal.svg)](https://github.com/libhal/libhal-stm32f1/issues)
[![Latest Version](https://libhal.github.io/libhal-stm32f1/latest_version.svg)](https://github.com/libhal/libhal-stm32f1/blob/main/conanfile.py)

libhal platform library for the stm32f1 series of microcontrollers by
STMicroelectronics.

## 📚 Software APIs & Usage

To learn about the available drivers and APIs see the
[Doxygen](https://libhal.github.io/libhal-stm32f1/api)
documentation page or look at the
[`include/libhal-stm32f1`](https://github.com/libhal/libhal-stm32f1/tree/main/include/libhal-stm32f1)
directory.

To see how each driver is used see the
[`demos/`](https://github.com/libhal/libhal-stm32f1/tree/main/demos) directory.

## 🧰 Setup

Following the
[🚀 Getting Started](https://libhal.github.io/2.1/getting_started/)
instructions.

## 📡 Installing Profiles

Profiles define which platform you mean to build your project against. These
profiles are needed for code and demos in this repo and for applications that
wish to execute on an stm32f1 device.

```bash
conan config install -sf conan/profiles/ -tf profiles https://github.com/libhal/libhal-armcortex.git
conan config install -sf conan/profiles/ -tf profiles https://github.com/libhal/libhal-stm32f1.git
```

Note that running these functions is safe. THey simply overwrite the old files
with the latest files. So running this for `libhal-armcortex` between this and
other platform libraries is fine.

## 🏗️ Building Demos

To build demos, start at the root of the repo and execute the following command:

```bash
conan build demos -pr stm32f103c8 -s build_type=Debug
```

This will build the demos for the `stm32f103c8` microcontroller in `Debug` mode.
Replace `stm32f103c8` with any of the other profiles. The Blue Pill board and
STM32F1 MicroMod use the profile `stm32f103c8`. Available profiles can be found
in
[`conan/profile/`](https://github.com/libhal/libhal-stm32f1/tree/main/conan/profile/).

## 💾 Flashing/Programming

There are a few ways to flash an LPC40 series MCU. The recommended methods are
via serial UART or using a debugger JTAG/SWD.

### Using Serial/UART over stm32loader

`stm32loader` is a script for programming and flashing LPC40 series chips over
serial/UART. Using it will require a USB to serial/uart adaptor.

For more information, please refer to the README of
[stm32loader](https://pypi.org/project/stm32loader/).

To install stm32loader:

```bash
python3 -m pip install stm32loader
```

To flash command is:

```bash
stm32loader -p /dev/tty.usbserial-10 -e -w -v demos/build/stm32f103c8/Debug/blinker.elf.bin
```

- Replace `demos/build/stm32f103c8/Debug/blinker.elf.bin` with the path to the binary you'd like
  to flash.
- Replace `/dev/tty.usbserial-10` with the path to your serial port on your
  machine.
  - Don't know which serial port to use? Use this guide from the MATLAB docs
    to your port for your operating system. Simply ignore that its made for
    Arduino, this guide will work for any serial USB device: [Find Arduino Port on
    Windows, Mac, and
    Linux](https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html)

### Using JTAG/SWD over PyOCD

`PyOCD` is a debugging interface for programming and also debugging ARM Cortex M
processor devices over JTAG and SWD.

This will require a JTAG or SWD debugger. The recommended debugger for the
LPC40 series of devices is the STLink v2 (cheap variants can be found on
Amazon).

See [PyOCD Installation Page](https://pyocd.io/docs/installing) for installation
details.

For reference the flashing command is:

```bash
pyocd flash --target stm32f103rc build/stm32f103c8/Debug/blinker.elf.bin
```

Note that target `stm32f103rc` works for all stm32f1 series microcontrollers.

## 📦 Adding `libhal-stm32f1` to your project

This section assumes you are using the
[`libhal-starter`](https://github.com/libhal/libhal-starter)
project.

Make sure to add the following options and default options to your app's
`ConanFile` class:

```python
    options = {"platform": ["ANY"]}
    default_options = {"platform": "unspecified"}
```

Add the following to your `requirements()` method:

```python
    def requirements(self):
        if str(self.options.platform).startswith("stm32f1"):
            self.requires("libhal-stm32f1/[^2.0.0]")
```

The version number can be changed to whatever is appropriate for your
application. If you don't know, using the latest is usually a good choice.

The CMake from the starter project will already be ready to support the new
platform library. No change needed.

To perform a test build simple run `conan build` as is done above with the
desired target platform profile.

## 🏁 Startup & Initialization

The `initialize_processor()` function for all platforms should do the following:

```C++
#include <libhal-armcortex/startup.hpp>

hal::status initialize_processor()
{
  hal::cortex_m::initialize_data_section();

  return hal::success();
}
```

### 🏎️ Setting Clock Speed

To setting the CPU clock speed to the maximum of 120MHz, include the line below,
with the rest of the includes:

```C++
#include <libhal-stm32f1/clock.hpp>
```

Next run the following command but replace `12.0_MHz` with the crystal
oscillator frequency connected to the microcontroller. This command REQUIRES
that there be a crystal oscillator attached to the microcontroller. Calling
this without the oscillator will cause the device to freeze as it will attempt
to use a clock that does not exist.

```C++
// NOT AVAILABLE YET...
```

#### 🕰️ Detailed Clock Tree Control

Coming soon...

## 🔎 On Chip Software Debugging

### Using PyOCD (✅ RECOMMENDED)

In one terminal:

```bash
pyocd gdbserver --target=stm32f103rc --persist
```

In another terminal:

```bash
arm-none-eabi-gdb demos/build/stm32f103c8/blinker.elf -ex "target remote :3333"
```

Replace `demos/build/stm32f103c8/blinker.elf` with the path to the elf file you'd
like to use for the debugging session.

### Using OpenOCD

Coming soon... (its more complicated)

## :busts_in_silhouette: Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for details.

## License

Apache 2.0; see [`LICENSE`](LICENSE) for details.

## Disclaimer

This project is not an official Google project. It is not supported by
Google and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
