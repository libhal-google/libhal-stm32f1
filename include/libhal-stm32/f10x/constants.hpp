#pragma once

#include <cstdint>

namespace hal::stm32::f10x {

/// Number of bits between each enable register
static constexpr uint32_t bus_id_offset = 32;
/// Bit position of AHB
static constexpr uint32_t ahb_bus = bus_id_offset * 0;
/// Bit position of APB1
static constexpr uint32_t apb1_bus = bus_id_offset * 1;
/// Bit position of AHB2
static constexpr uint32_t apb2_bus = bus_id_offset * 2;
/// Bit position of systems outside of any bus
static constexpr uint32_t beyond_bus = bus_id_offset * 3;

/// List of each peripheral and their power on id number for this platform
enum class peripheral : std::uint8_t
{
  dma1 = ahb_bus + 0,
  dma2 = ahb_bus + 1,
  sram = ahb_bus + 2,
  flitf = ahb_bus + 4,
  crc = ahb_bus + 6,
  fsmc = ahb_bus + 8,
  sdio = ahb_bus + 10,

  timer2 = apb1_bus + 0,
  timer3 = apb1_bus + 1,
  timer4 = apb1_bus + 2,
  timer5 = apb1_bus + 3,
  timer6 = apb1_bus + 4,
  timer7 = apb1_bus + 5,
  timer12 = apb1_bus + 6,
  timer13 = apb1_bus + 7,
  timer14 = apb1_bus + 8,
  window_watchdog = apb1_bus + 11,
  spi2 = apb1_bus + 14,
  spi3 = apb1_bus + 15,
  usart2 = apb1_bus + 17,
  usart3 = apb1_bus + 18,
  uart4 = apb1_bus + 19,
  uart5 = apb1_bus + 20,
  i2c1 = apb1_bus + 21,
  i2c2 = apb1_bus + 22,
  usb = apb1_bus + 23,
  can1 = apb1_bus + 25,
  backup_clock = apb1_bus + 27,
  power = apb1_bus + 28,
  dac = apb1_bus + 29,

  afio = apb2_bus + 0,
  gpioa = apb2_bus + 2,
  gpiob = apb2_bus + 3,
  gpioc = apb2_bus + 4,
  gpiod = apb2_bus + 5,
  gpioe = apb2_bus + 6,
  gpiof = apb2_bus + 7,
  gpiog = apb2_bus + 8,
  adc1 = apb2_bus + 9,
  adc2 = apb2_bus + 10,
  timer1 = apb2_bus + 11,
  spi1 = apb2_bus + 12,
  timer8 = apb2_bus + 13,
  usart1 = apb2_bus + 14,
  adc3 = apb2_bus + 15,
  timer9 = apb2_bus + 19,
  timer10 = apb2_bus + 20,
  timer11 = apb2_bus + 21,

  cpu = beyond_bus + 0,
  system_timer = beyond_bus + 1,
  i2s = beyond_bus + 2,
};

/// List of interrupt request numbers for this platform
enum class irq : std::uint16_t
{
  /// Window WatchDog
  window_watchdog = 0,
  /// PVD through EXTI Line detection
  pvd = 1,
  /// Tamper
  tamper = 2,
  /// RTC
  rtc = 3,
  /// FLASH
  flash = 4,
  /// RCC
  rcc = 5,
  /// EXTI Line0
  exti0 = 6,
  /// EXTI Line1
  exti1 = 7,
  /// EXTI Line2
  exti2 = 8,
  /// EXTI Line3
  exti3 = 9,
  /// EXTI Line4
  exti4 = 10,
  /// DMA1 Channel 1
  dma1_channel1 = 11,
  /// DMA1 Channel 2
  dma1_channel2 = 12,
  /// DMA1 Channel 3
  dma1_channel3 = 13,
  /// DMA1 Channel 4
  dma1_channel4 = 14,
  /// DMA1 Channel 5
  dma1_channel5 = 15,
  /// DMA1 Channel 6
  dma1_channel6 = 16,
  /// DMA1 Channel 7
  dma1_channel7 = 17,
  /// ADC1
  adc1 = 18,
  /// ADC1 and ADC2
  adc1_2 = 18,
  /// USB Device High Priority or CAN1 TX
  usb_hp_can1_tx = 19,
  /// USB Device High Priority or CAN1 TX
  can1_tx = 19,
  /// USB Device Low Priority or CAN1 RX0
  usb_lp_can1_rx0 = 20,
  /// USB Device Low Priority or CAN1 RX0
  can1_rx0 = 20,
  /// CAN1 RX1
  can1_rx1 = 21,
  /// CAN1 SCE
  can1_sce = 22,
  /// External Line[9:5]
  exti9_5 = 23,
  /// TIM1 Break
  tim1_brk = 24,
  /// TIM1 Break and TIM15
  tim1_brk_tim15 = 24,
  /// TIM1 Break and TIM9
  tim1_brk_tim9 = 24,
  /// TIM1 Update
  tim1_up = 25,
  /// TIM1 Update and TIM16
  tim1_up_tim16 = 25,
  /// TIM1 Update and TIM10
  tim1_up_tim10 = 25,
  /// TIM1 Trigger and Commutation
  tim1_trg_com = 26,
  /// TIM1 Trigger and Commutation
  tim1_trg_com_tim11 = 26,
  /// TIM1 Capture Compare
  tim1_cc = 27,
  /// TIM2
  tim2 = 28,
  /// TIM3
  tim3 = 29,
  /// TIM4
  tim4 = 30,
  /// I2C1 Event
  i2c1_ev = 31,
  /// I2C1 Error
  i2c1_er = 32,
  /// I2C2 Event
  i2c2_ev = 33,
  /// I2C2 Error
  i2c2_er = 34,
  /// SPI1
  spi1 = 35,
  /// SPI2
  spi2 = 36,
  /// USART1
  usart1 = 37,
  /// USART2
  usart2 = 38,
  /// USART3
  usart3 = 39,
  /// External Line[15:10]
  exti15_10 = 40,
  /// RTC Alarm through EXTI Line
  rtcalarm = 41,
  /// USB Device WakeUp
  usbwakeup = 42,
  /// HDMI-CEC
  cec = 42,
  /// USB OTG FS WakeUp
  otg_fs_wkup = 42,
  /// TIM8 Break
  tim8_brk = 43,
  /// TIM12
  tim12 = 43,
  /// TIM8 Break and TIM12
  tim8_brk_tim12 = 43,
  /// TIM8 Update
  tim8_up = 44,
  /// TIM13
  tim13 = 44,
  /// TIM8 Update and TIM13
  tim8_up_tim13 = 44,
  /// TIM8 Trigger and Commutation
  tim8_trg_com = 45,
  /// TIM14
  tim14 = 45,
  /// TIM8 Trigger and Commutation
  tim8_trg_com_tim14 = 45,
  /// TIM8 Capture Compare
  tim8_cc = 46,
  /// ADC3
  adc3 = 47,
  /// FSMC
  fsmc = 48,
  /// SDIO
  sdio = 49,
  /// TIM5
  tim5 = 50,
  /// SPI3
  spi3 = 51,
  /// UART4
  uart4 = 52,
  /// UART5
  uart5 = 53,
  /// TIM6 and DAC underrun
  tim6_dac = 54,
  /// TIM6
  tim6 = 54,
  /// TIM7
  tim7 = 55,
  /// DMA2 Channel 1
  dma2_channel1 = 56,
  /// DMA2 Channel 2
  dma2_channel2 = 57,
  /// DMA2 Channel 3
  dma2_channel3 = 58,
  /// DMA2 Channel 4 and Channel 5
  dma2_channel4_5 = 59,
  /// DMA2 Channel 4
  dma2_channel4 = 59,
  /// DMA2 Channel 5
  dma2_channel5 = 60,
  /// Ethernet
  eth = 61,
  /// Ethernet Wakeup through EXTI line
  eth_wkup = 62,
  /// CAN2 TX
  can2_tx = 63,
  /// CAN2 RX0
  can2_rx0 = 64,
  /// CAN2 RX1
  can2_rx1 = 65,
  /// CAN2 SCE
  can2_sce = 66,
  /// USB OTG FS
  otg_fs = 67,
  max,
};
/// Error types for stm32f10x
enum class error_t
{
  max,
};
}  // namespace hal::stm32::f10x
