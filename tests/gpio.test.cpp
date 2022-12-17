#include "gpio.hpp"

#include <utility>

#include <libcore/testing/testing_frameworks.hpp>

namespace sjsu::stm32f10x {
namespace {
sjsu::bit::Mask Mask4Bit(uint8_t pin_position)
{
  return {
    .position = static_cast<uint32_t>(pin_position * 4),
    .width = 4,
  };
}

IRQn GetIrqForPin(uint8_t pin)
{
  switch (pin) {
    case 0:
      return stm32f10x::EXTI0_IRQn;
      break;
    case 1:
      return stm32f10x::EXTI1_IRQn;
      break;
    case 2:
      return stm32f10x::EXTI2_IRQn;
      break;
    case 3:
      return stm32f10x::EXTI3_IRQn;
      break;
    case 4:
      return stm32f10x::EXTI4_IRQn;
      break;
    case 5:
      [[fallthrough]];
    case 6:
      [[fallthrough]];
    case 7:
      [[fallthrough]];
    case 8:
      [[fallthrough]];
    case 9:
      return stm32f10x::EXTI9_5_IRQn;
      break;
    case 10:
      [[fallthrough]];
    case 11:
      [[fallthrough]];
    case 12:
      [[fallthrough]];
    case 13:
      [[fallthrough]];
    case 14:
      [[fallthrough]];
    case 15:
      return stm32f10x::EXTI15_10_IRQn;
      break;
    default:
      return static_cast<stm32f10x::IRQn>(0xFFFF);
  }
}
}  // namespace

TEST_CASE("Testing stm32f10x Gpio")
{
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  InterruptController::SetPlatformController(&mock_interrupt_controller.get());

  GPIO_TypeDef local_gpio_a;
  GPIO_TypeDef local_gpio_b;
  GPIO_TypeDef local_gpio_c;
  GPIO_TypeDef local_gpio_d;
  GPIO_TypeDef local_gpio_e;
  GPIO_TypeDef local_gpio_f;
  GPIO_TypeDef local_gpio_g;
  AFIO_TypeDef local_afio;
  EXTI_TypeDef local_exti;

  testing::ClearStructure(&local_gpio_a);
  testing::ClearStructure(&local_gpio_b);
  testing::ClearStructure(&local_gpio_c);
  testing::ClearStructure(&local_gpio_d);
  testing::ClearStructure(&local_gpio_e);
  testing::ClearStructure(&local_gpio_f);
  testing::ClearStructure(&local_gpio_g);
  testing::ClearStructure(&local_afio);
  testing::ClearStructure(&local_exti);

  // The stm32f10x::Gpio class uses the stm32f10x::Pin registers directly
  Gpio::gpio[0] = &local_gpio_a;
  Gpio::gpio[1] = &local_gpio_b;
  Gpio::gpio[2] = &local_gpio_c;
  Gpio::gpio[3] = &local_gpio_d;
  Gpio::gpio[4] = &local_gpio_e;
  Gpio::gpio[5] = &local_gpio_f;
  Gpio::gpio[6] = &local_gpio_g;
  Gpio::afio = &local_afio;

  Gpio::external_interrupt = &local_exti;

  struct TestStruct_t
  {
    std::pair<char, uint8_t> gpio;
    GPIO_TypeDef& reg;
    sjsu::ResourceID id;
  };

  std::array<TestStruct_t, 15> test = {
    TestStruct_t{
      .gpio = std::make_pair('A', 0),  // A
      .reg = local_gpio_a,
      .id = stm32f10x::kGpioA,
    },
    TestStruct_t{
      .gpio = std::make_pair('A', 4),  // Middle of first half word
      .reg = local_gpio_a,
      .id = stm32f10x::kGpioA,
    },
    TestStruct_t{
      .gpio = std::make_pair('B', 0),  // B
      .reg = local_gpio_b,
      .id = stm32f10x::kGpioB,
    },
    TestStruct_t{
      .gpio = std::make_pair('B', 7),  // End of first half word
      .reg = local_gpio_b,
      .id = stm32f10x::kGpioB,
    },
    TestStruct_t{
      .gpio = std::make_pair('C', 0),  // C
      .reg = local_gpio_c,
      .id = stm32f10x::kGpioC,
    },
    TestStruct_t{
      .gpio = std::make_pair('C', 8),  // First of last half word
      .reg = local_gpio_c,
      .id = stm32f10x::kGpioC,
    },
    TestStruct_t{
      .gpio = std::make_pair('D', 0),  // D
      .reg = local_gpio_d,
      .id = stm32f10x::kGpioD,
    },
    TestStruct_t{
      .gpio = std::make_pair('D', 12),  // Middle of last half word
      .reg = local_gpio_d,
      .id = stm32f10x::kGpioD,
    },
    TestStruct_t{
      .gpio = std::make_pair('E', 0),  // E
      .reg = local_gpio_e,
      .id = stm32f10x::kGpioE,
    },
    TestStruct_t{
      .gpio = std::make_pair('E', 15),  // Last of last half word
      .reg = local_gpio_e,
      .id = stm32f10x::kGpioE,
    },
    TestStruct_t{
      .gpio = std::make_pair('F', 0),  // F
      .reg = local_gpio_f,
      .id = stm32f10x::kGpioF,
    },
    TestStruct_t{
      .gpio = std::make_pair('G', 0),  // G
      .reg = local_gpio_g,
      .id = stm32f10x::kGpioG,
    },
    TestStruct_t{
      .gpio = std::make_pair('G', 1),  // G
      .reg = local_gpio_g,
      .id = stm32f10x::kGpioG,
    },
    TestStruct_t{
      .gpio = std::make_pair('G', 2),  // G
      .reg = local_gpio_g,
      .id = stm32f10x::kGpioG,
    },
    TestStruct_t{
      .gpio = std::make_pair('G', 3),  // G
      .reg = local_gpio_g,
      .id = stm32f10x::kGpioG,
    },
  };

  SECTION("Initialize()")
  {
    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at Pin " << test_subject.gpio);

      // Exercise
      gpio.Initialize();

      // Verify
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Using(test_subject.id));
      mock_system_controller.ClearInvocationHistory();
    }
  }

  SECTION("SetDirection()")
  {
    constexpr uint8_t kInputFloatingCode = 0b0100;
    constexpr uint8_t kOutputFullSpeedCode = 0b0011;

    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at index: " << test_subject.gpio);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.CRL = 0xFFFF'FFFF;
      test_subject.reg.CRH = 0xFFFF'FFFF;

      // Exercise
      gpio.settings.Floating();
      gpio.Initialize();
      gpio.SetAsInput();

      // Exercise: Combine the two registers into 1 variable to make extraction
      //           easier.
      uint64_t crh = test_subject.reg.CRH;
      uint64_t crl = test_subject.reg.CRL;
      uint64_t cr = (crh << 32) | crl;

      // Verify
      CHECK(kInputFloatingCode ==
            bit::Extract(cr, Mask4Bit(test_subject.gpio.second)));
    }

    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at index: " << test_subject.gpio);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.CRL = 0xFFFF'FFFF;
      test_subject.reg.CRH = 0xFFFF'FFFF;

      // Exercise
      gpio.Initialize();
      gpio.SetAsOutput();

      // Exercise: Combine the two registers into 1 variable to make extraction
      //           easier. They should contain the newly updated control
      //           information.
      uint64_t crh = test_subject.reg.CRH;
      uint64_t crl = test_subject.reg.CRL;
      uint64_t cr = (crh << 32) | crl;

      // Verify
      CHECK(kOutputFullSpeedCode ==
            bit::Extract(cr, Mask4Bit(test_subject.gpio.second)));
    }
  }
  SECTION("Set()")
  {
    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at index: " << test_subject.gpio);
      // Setup: Set register to all zeros.
      //        A 1 in bits [0:15] will set the output HIGH.
      //        A 1 in bits [16:31] will set the output LOW.
      test_subject.reg.BSRR = 0;

      // Exercise
      gpio.SetLow();

      // Verify
      CHECK((1 << (test_subject.gpio.second + 16)) == test_subject.reg.BSRR);
    }

    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at index: " << test_subject.gpio);
      // Setup: Set register to all zeros.
      //        A 1 in bits [0:15] will set the output HIGH.
      //        A 1 in bits [16:31] will set the output LOW.
      test_subject.reg.BSRR = 0;

      // Exercise
      gpio.SetHigh();

      // Verify
      CHECK((1 << test_subject.gpio.second) == test_subject.reg.BSRR);
    }
  }

  SECTION("Toggle()")
  {
    for (auto& test_subject : test) {
      // Setup
      sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                 test_subject.gpio.second);
      INFO("Failure at index: " << test_subject.gpio);
      // Setup: Initialize output register as zero
      test_subject.reg.ODR = 0;

      // Exercise
      gpio.Toggle();

      bool should_be_set =
        bit::Read(test_subject.reg.ODR, test_subject.gpio.second);

      gpio.Toggle();
      bool should_be_cleared =
        bit::Read(test_subject.reg.ODR, test_subject.gpio.second);

      // Verify
      CHECK(should_be_set == true);
      CHECK(should_be_cleared == false);
    }
  }

  SECTION("Read()")
  {
    constexpr std::array<uint32_t, 2> kIdr = {
      0x5555'5555,
      0xAAAA'AAAA,
    };

    for (uint32_t i = 0; i < kIdr.size(); i++) {
      for (auto& test_subject : test) {
        test_subject.reg.IDR = kIdr[i];
        // Setup
        sjsu::stm32f10x::Gpio gpio(test_subject.gpio.first,
                                   test_subject.gpio.second);

        INFO("Failure at index: " << test_subject.gpio
                                  << " IDR: " << test_subject.reg.IDR);

        // Setup: Initialize output register as all 1s
        bool expected_read =
          bit::Read(test_subject.reg.IDR, test_subject.gpio.second);

        // Exercise + Verify
        CHECK(expected_read == gpio.Read());
      }
    }
  }

  /*
  SECTION("AttachInterrupt() + InterruptHandler()")
  {
    for (auto edge : {
             Gpio::Edge::kFalling,
             Gpio::Edge::kRising,
             Gpio::Edge::kBoth,
         })
    {
      for (uint32_t j = 0; j < test.size(); j++)
      {
        // Setup
        INFO("Failure at index: " << j);

        // Setup: Clear the EXTI register
        testing::ClearStructure(&local_exti);
        testing::ClearStructure(&local_afio);

        // Setup: Create shorthand variables for port, pin and IRQ
        uint8_t pin       = test[j].gpio.second;
        uint8_t port      = test[j].gpio.GetPort();
        auto expected_irq = GetIrqForPin(pin);

        // Setup: A function to determine if the InterruptHandler() is calling
        // the
        //        correct callback.
        bool callback_was_called   = false;
        InterruptCallback callback = [&callback_was_called]()
        { callback_was_called = true; };

        // Setup: The expected interrupt registration information to be used
        //        when AttachInterrupt() is called.
        auto expected_registration = InterruptController::RegistrationInfo_t{
          .interrupt_request_number = expected_irq,
          .interrupt_handler        = Gpio::InterruptHandler,
        };

        // Setup: Store the EXTICR control register into a variable to make the
        //        code cleaner.
        volatile uint32_t * control = &local_afio.EXTICR[pin / 4];

        // Setup: Define the mask within the EXTICR register for code clarity.
        auto interrupt_mask = bit::Mask{
          .position = static_cast<uint32_t>((pin * 4) % 16),
          .width    = 4,
        };

        // Setup: Set Pending Register (PR) for this interrupt to a 1, asserting
        //        that there was an interrupt event for this pin. This should
        //        cause Gpio::InterruptHandler() to call `callback()`.
        local_exti.PR = (1 << pin);

        // Exercise
        test[j].gpio.AttachInterrupt(callback, edge);

        // Exercise: Call the handler directly
        Gpio::InterruptHandler();

        // Verify
        CHECK(callback_was_called);

        if (edge == Gpio::Edge::kBoth)
        {
          CHECK(local_exti.RTSR == 1 << pin);
          CHECK(local_exti.FTSR == 1 << pin);
        }
        else if (edge == Gpio::Edge::kRising)
        {
          CHECK(local_exti.RTSR == 1 << pin);
          CHECK(local_exti.FTSR == 0);
        }
        else if (edge == Gpio::Edge::kFalling)
        {
          CHECK(local_exti.RTSR == 0);
          CHECK(local_exti.FTSR == 1 << pin);
        }

        CHECK(local_exti.IMR == 1 << pin);
        CHECK(*control == bit::Value(0).Insert(port - 'A', interrupt_mask));
        CHECK(local_exti.PR == (1 << pin));
        Verify(Method(mock_interrupt_controller, Enable)
                   .Using(expected_registration));
      }
    }
  }

  SECTION("DetachInterrupt()")
  {
    for (uint32_t j = 0; j < test.size(); j++)
    {
      // Setup
      INFO("Failure at index: " << j);

      // Setup: Create shorthand variables for port, pin and IRQ
      uint8_t pin = test[j].gpio.second;

      // Setup: Set both registers to all 1s
      local_exti.RTSR = std::numeric_limits<decltype(local_exti.RTSR)>::max();
      local_exti.FTSR = std::numeric_limits<decltype(local_exti.FTSR)>::max();

      // Setup: The register should contain all 1s except for a single 0 in the
      //        pin bit location.
      uint32_t expected_result = ~(1 << pin);

      // Exercise
      test[j].gpio.DetachInterrupt();

      // Verify
      // Verify: That the specific
      CHECK(local_exti.RTSR == expected_result);
      CHECK(local_exti.FTSR == expected_result);
    }
  } */
}
}  // namespace sjsu::stm32f10x
