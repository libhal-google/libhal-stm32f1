#pragma once

#include <libstm32f10x/platform/stm32f10x.h>

#include <cstdint>
#include <libcore/peripherals/gpio.hpp>
#include <libcore/peripherals/interrupt.hpp>
#include <libcore/peripherals/system_controller.hpp>
#include <libcore/utility/error_handling.hpp>
#include <libcore/utility/math/bit.hpp>
#include <libstm32f10x/platform/constants.hpp>

namespace sjsu::stm32f10x
{
/// An abstract interface for General Purpose I/O
class Gpio : public sjsu::Gpio
{
 public:
  /// The highest pin value for stm32f10x MCUs is 0 to 15, thus we have a limit
  /// of 15 pins per port.
  static constexpr uint8_t kPinMaximum = 16;

  /// Lookup table that holds developer gpio interrupt handlers.
  inline static InterruptCallback handlers[kPinMaximum];

  /// Pointer to the external interrupt peripheral
  inline static EXTI_TypeDef * external_interrupt = EXTI;

  /// Pointer to the alternative function I/O register
  static inline AFIO_TypeDef * afio = AFIO;

  /// List of GPIO peripherals
  static inline std::array<GPIO_TypeDef *, 7> gpio = {
    GPIOA,  // => 'A'
    GPIOB,  // => 'B'
    GPIOC,  // => 'C'
    GPIOD,  // => 'D'
    GPIOE,  // => 'E'
    GPIOF,  // => 'F'
    GPIOG,  // => 'G'
  };

  /// The GPIO pins PB3, PB4, and PA15 are default initalized to be used for
  /// JTAG purposes. If you are using SWD and want to use these pins as GPIO or
  /// as other alternative functions, this function MUST be called.
  static void ReleaseJTAGPins()
  {
    auto & system = SystemController::GetPlatformController();

    // Enable the usage of alternative pin configurations
    system.PowerUpPeripheral(kAFIO);

    // Set the JTAG Release
    afio->MAPR =
        sjsu::bit::Insert(afio->MAPR, 0b010, sjsu::bit::MaskFromRange(24, 26));
  }

  /// Maps the CONFIG flags for each pin use case
  struct PinConfigMapping_t
  {
    /// Configuration bit 1
    uint8_t CNF1;
    /// Configuration bit 0
    uint8_t CNF0;
    /// Mode bits
    uint8_t MODE;
    /// Output data register
    uint8_t PxODR;
  };

  /// @param port - must be a capitol letter from 'A' to 'G'
  /// @param pin - must be between 0 to 15
  constexpr Gpio(uint8_t port, uint8_t pin) : sjsu::Gpio(port, pin) {}

  void ModuleInitialize() override
  {
    if (settings.function > 2)
    {
      throw Exception(
          std::errc::invalid_argument,
          "Only functions 0 (meaning General Purpose Output) or 1 (meaning "
          "alternative function) or 2 (meaning General Purpose Floating Input) "
          "allowed!");
    }

    auto & system = SystemController::GetPlatformController();

    // Enable the usage of alternative pin configurations
    system.PowerUpPeripheral(kAFIO);

    switch (GetPort())
    {
      case 'A': system.PowerUpPeripheral(kGpioA); break;
      case 'B': system.PowerUpPeripheral(kGpioB); break;
      case 'C': system.PowerUpPeripheral(kGpioC); break;
      case 'D': system.PowerUpPeripheral(kGpioD); break;
      case 'E': system.PowerUpPeripheral(kGpioE); break;
      case 'F': system.PowerUpPeripheral(kGpioF); break;
      case 'G': system.PowerUpPeripheral(kGpioG); break;
    }

    static constexpr PinConfigMapping_t kPushPullGpioOutput = {
      .CNF1  = 0,
      .CNF0  = 0,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kOpenDrainGpioOutput = {
      .CNF1  = 0,
      .CNF0  = 1,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kPushPullAlternativeOutput = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kOpenDrainAlternativeOutput = {
      .CNF1  = 1,
      .CNF0  = 1,
      .MODE  = 0b11,  // Default to high speed 50 MHz
      .PxODR = 0b0,   // Default to 0 LOW Voltage
    };

    static constexpr PinConfigMapping_t kInputAnalog = {
      .CNF1  = 0,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Don't care
    };

    static constexpr PinConfigMapping_t kInputFloat = {
      .CNF1  = 0,
      .CNF0  = 1,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Don't care
    };

    static constexpr PinConfigMapping_t kInputPullDown = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b0,  // Pull Down
    };

    static constexpr PinConfigMapping_t kInputPullUp = {
      .CNF1  = 1,
      .CNF0  = 0,
      .MODE  = 0b00,
      .PxODR = 0b1,  // Pull Up
    };

    PinConfigMapping_t mapping;

    if (settings.as_analog)
    {
      mapping = kInputAnalog;
    }
    else if (settings.resistor == PinSettings_t::Resistor::kPullDown)
    {
      mapping = kInputPullDown;
    }
    else if (settings.resistor == PinSettings_t::Resistor::kPullUp)
    {
      mapping = kInputPullUp;
    }
    else if (settings.function == 0 && settings.open_drain == false)
    {
      mapping = kPushPullGpioOutput;
    }
    else if (settings.function == 0 && settings.open_drain == true)
    {
      mapping = kOpenDrainGpioOutput;
    }
    else if (settings.function == 1 && settings.open_drain == false)
    {
      mapping = kPushPullAlternativeOutput;
    }
    else if (settings.function == 1 && settings.open_drain == true)
    {
      mapping = kOpenDrainAlternativeOutput;
    }
    else if (settings.function == 2)
    {
      mapping = kInputFloat;
    }
    else
    {
      throw sjsu::Exception(
          std::errc::not_supported,
          "This pin configuration is not supported by the STM32F10x platform");
    }

    constexpr auto kCNF1 = bit::MaskFromRange(3);
    constexpr auto kCNF0 = bit::MaskFromRange(2);
    constexpr auto kMODE = bit::MaskFromRange(0, 1);

    uint32_t config = bit::Value(0)
                          .Insert(mapping.CNF1, kCNF1)
                          .Insert(mapping.CNF0, kCNF0)
                          .Insert(mapping.MODE, kMODE)
                          .To<uint32_t>();

    SetConfig(config);
    Port()->ODR = bit::Insert(Port()->ODR, mapping.PxODR, GetPin());
  }

  void SetDirection(Direction direction) override
  {
    // Set to GPIO mode
    if (direction == Direction::kInput)
    {
      settings.function = 2;  // input function
    }
    else
    {
      settings.function = 0;  // output function
    }

    Initialize();
  }

  void Set(State output) override
  {
    if (output == State::kHigh)
    {
      // The first 16 bits of the register set the output state
      Port()->BSRR = (1 << GetPin());
    }
    else
    {
      // The last 16 bits of the register reset the output state
      Port()->BSRR = (1 << (GetPin() + 16));
    }
  }

  void Toggle() override
  {
    Port()->ODR = (Port()->ODR ^ (1 << GetPin()));
  }

  bool Read() override
  {
    return bit::Read(Port()->IDR, GetPin());
  }

  /// The gpio interrupt handler that calls the attached interrupt callbacks.
  static void InterruptHandler()
  {
    // Find the pin that triggered this interrupt by reading the Pending
    // Register. Each bit location number represents the pin number that
    // triggered this interrupt.
    // We can count the number of zeros from the right to return the bit number
    // of the least significant set bit in the register, which will be at least
    // one of the pins that triggered this handler.
    int pin = __builtin_ctz(external_interrupt->PR);

    // Use the pin to lookup and run the associated callback
    handlers[pin]();

    // Clear the pending interrupt by writing a 1 back to the PR register.
    external_interrupt->PR = (1 << pin);
  }

  void AttachInterrupt(InterruptCallback callback, Edge edge) override
  {
    // Clear both falling and raising edges bits
    // They will be assigned in the conditionals below
    DetachInterrupt();

    // Add callback to list of handlers
    handlers[GetPin()] = callback;

    if (edge == Edge::kBoth || edge == Edge::kRising)
    {
      external_interrupt->RTSR = bit::Set(external_interrupt->RTSR, GetPin());
    }
    if (edge == Edge::kBoth || edge == Edge::kFalling)
    {
      external_interrupt->FTSR = bit::Set(external_interrupt->FTSR, GetPin());
    }

    // Enable interrupts for this particular pin
    external_interrupt->IMR = bit::Set(external_interrupt->IMR, GetPin());

    // Fetch the external interrupt control register from the AFIO peripheral
    // There are 4 EXTICR registers which only use the first 16 bits of the
    // 32-bit register.
    // We have 16 EXTI interrupt signals which can be assigned to any of the
    // ports A, B, ..., G.
    // Divide the pin number by 4 to select the EXTICR register in the array
    // to update.
    volatile uint32_t * control = &afio->EXTICR[GetPin() / 4];

    // The location within the EXTICR[x] register for the port select
    // information
    auto interrupt_alternative_function_mask = bit::Mask{
      .position = static_cast<uint32_t>((GetPin() * 4) % 16),
      .width    = 4,
    };

    // Assign the control port value, which is equal to the port value minus
    // 'A', thus A would be 0, B would be 1, etc.
    *control = bit::Insert(
        *control, GetPort() - 'A', interrupt_alternative_function_mask);

    // Enable interrupts for this external interrupt line
    switch (GetPin())
    {
      case 0:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI0_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 1:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI1_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 2:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI2_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 3:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI3_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 4:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI4_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 5: [[fallthrough]];
      case 6: [[fallthrough]];
      case 7: [[fallthrough]];
      case 8: [[fallthrough]];
      case 9:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI9_5_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 10: [[fallthrough]];
      case 11: [[fallthrough]];
      case 12: [[fallthrough]];
      case 13: [[fallthrough]];
      case 14: [[fallthrough]];
      case 15:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI15_10_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      default:
      {
        throw Exception(std::errc::invalid_argument,
                        "Pin must be between 0-15");
      }
    }
  }

  void DetachInterrupt() override
  {
    // No need to Enable/Disable the interrupt via the interrupt controller,
    // especially since you would need logic to determine if this is the last
    // pin used for a particular interrupt request line. For example,
    // EXTI15_10_IRQn can only be disabled if we know that no other pins need
    // that IRQ enabled. To simply the logic simply disabling falling edge or
    // rising edge detection is all that is needed.
    external_interrupt->RTSR = bit::Clear(external_interrupt->RTSR, GetPin());
    external_interrupt->FTSR = bit::Clear(external_interrupt->FTSR, GetPin());
  }

  ~Gpio()
  {
    DetachInterrupt();
  }

 private:
  /// Returns the a pointer the gpio port.
  GPIO_TypeDef * Port() const
  {
    return gpio[GetPort() - 'A'];
  }

  /// Returns a bit mask indicating where the config bits are in the config
  /// registers.
  bit::Mask Mask() const
  {
    return {
      .position = static_cast<uint32_t>((GetPin() * 4) % 32),
      .width    = 4,
    };
  }

  /// Returns the configuration control register for the specific pin.
  /// Pins 0 - 7 are in CRL and Pins 8 - 15 are in CRH.
  volatile uint32_t * Config() const
  {
    volatile uint32_t * config = &Port()->CRL;

    if (GetPin() > 7)
    {
      config = &Port()->CRH;
    }

    return config;
  }

  /// @return the 4 bits of this ports config.
  uint32_t GetConfig() const
  {
    return bit::Extract(*Config(), Mask());
  }

  /// Set this ports 4 bits configuration.
  void SetConfig(uint32_t value) const
  {
    *Config() = bit::Insert(*Config(), value, Mask());
  }
};

template <int port, int pin_number>
inline Gpio & GetGpio()
{
  static_assert(
      ('A' <= port && port <= 'I') && (0 <= pin_number && pin_number <= 15),
      "\n\n"
      "SJSU-Dev2 Compile Time Error:\n"
      "    stm32f10x: Port must be between 'A' and 'I' and pin must be\n"
      "    between 0 and 15!\n"
      "\n");

  static Gpio gpio(port, pin_number);
  return gpio;
}
}  // namespace sjsu::stm32f10x
