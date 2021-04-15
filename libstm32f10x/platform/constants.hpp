#pragma once

#include <libcore/platform/constants.hpp>

namespace sjsu::stm32f10x
{
// Bits per register
static constexpr int kBitsPerRegister = 32;
/// Bit position of AHB
static constexpr int kAHB = kBitsPerRegister * 0;
/// Bit position of APB1
static constexpr int kAPB1 = kBitsPerRegister * 1;
/// Bit position of AHB2
static constexpr int kAPB2 = kBitsPerRegister * 2;
/// Bit position of systems outside of any bus
static constexpr int kBeyond = kBitsPerRegister * 3;

//! @cond Doxygen_Suppress
static constexpr auto kDma1           = sjsu::ResourceID::Define<kAHB + 0>();
static constexpr auto kDma2           = sjsu::ResourceID::Define<kAHB + 1>();
static constexpr auto kSram           = sjsu::ResourceID::Define<kAHB + 2>();
static constexpr auto kFlitf          = sjsu::ResourceID::Define<kAHB + 4>();
static constexpr auto kCrc            = sjsu::ResourceID::Define<kAHB + 6>();
static constexpr auto kFsmc           = sjsu::ResourceID::Define<kAHB + 8>();
static constexpr auto kSdio           = sjsu::ResourceID::Define<kAHB + 10>();
static constexpr auto kTimer2         = sjsu::ResourceID::Define<kAPB1 + 0>();
static constexpr auto kTimer3         = sjsu::ResourceID::Define<kAPB1 + 1>();
static constexpr auto kTimer4         = sjsu::ResourceID::Define<kAPB1 + 2>();
static constexpr auto kTimer5         = sjsu::ResourceID::Define<kAPB1 + 3>();
static constexpr auto kTimer6         = sjsu::ResourceID::Define<kAPB1 + 4>();
static constexpr auto kTimer7         = sjsu::ResourceID::Define<kAPB1 + 5>();
static constexpr auto kTimer12        = sjsu::ResourceID::Define<kAPB1 + 6>();
static constexpr auto kTimer13        = sjsu::ResourceID::Define<kAPB1 + 7>();
static constexpr auto kTimer14        = sjsu::ResourceID::Define<kAPB1 + 8>();
static constexpr auto kWindowWatchdog = sjsu::ResourceID::Define<kAPB1 + 11>();
static constexpr auto kSpi2           = sjsu::ResourceID::Define<kAPB1 + 14>();
static constexpr auto kSpi3           = sjsu::ResourceID::Define<kAPB1 + 15>();
static constexpr auto kUsart2         = sjsu::ResourceID::Define<kAPB1 + 17>();
static constexpr auto kUsart3         = sjsu::ResourceID::Define<kAPB1 + 18>();
static constexpr auto kUart4          = sjsu::ResourceID::Define<kAPB1 + 19>();
static constexpr auto kUart5          = sjsu::ResourceID::Define<kAPB1 + 20>();
static constexpr auto kI2c1           = sjsu::ResourceID::Define<kAPB1 + 21>();
static constexpr auto kI2c2           = sjsu::ResourceID::Define<kAPB1 + 22>();
static constexpr auto kUsb            = sjsu::ResourceID::Define<kAPB1 + 23>();
static constexpr auto kCan1           = sjsu::ResourceID::Define<kAPB1 + 25>();
static constexpr auto kBackupClock    = sjsu::ResourceID::Define<kAPB1 + 27>();
static constexpr auto kPower          = sjsu::ResourceID::Define<kAPB1 + 28>();
static constexpr auto kDac            = sjsu::ResourceID::Define<kAPB1 + 29>();
static constexpr auto kAFIO           = sjsu::ResourceID::Define<kAPB2 + 0>();
static constexpr auto kGpioA          = sjsu::ResourceID::Define<kAPB2 + 2>();
static constexpr auto kGpioB          = sjsu::ResourceID::Define<kAPB2 + 3>();
static constexpr auto kGpioC          = sjsu::ResourceID::Define<kAPB2 + 4>();
static constexpr auto kGpioD          = sjsu::ResourceID::Define<kAPB2 + 5>();
static constexpr auto kGpioE          = sjsu::ResourceID::Define<kAPB2 + 6>();
static constexpr auto kGpioF          = sjsu::ResourceID::Define<kAPB2 + 7>();
static constexpr auto kGpioG          = sjsu::ResourceID::Define<kAPB2 + 8>();
static constexpr auto kAdc1           = sjsu::ResourceID::Define<kAPB2 + 9>();
static constexpr auto kAdc2           = sjsu::ResourceID::Define<kAPB2 + 10>();
static constexpr auto kTimer1         = sjsu::ResourceID::Define<kAPB2 + 11>();
static constexpr auto kSpi1           = sjsu::ResourceID::Define<kAPB2 + 12>();
static constexpr auto kTimer8         = sjsu::ResourceID::Define<kAPB2 + 13>();
static constexpr auto kUsart1         = sjsu::ResourceID::Define<kAPB2 + 14>();
static constexpr auto kAdc3           = sjsu::ResourceID::Define<kAPB2 + 15>();
static constexpr auto kTimer9         = sjsu::ResourceID::Define<kAPB2 + 19>();
static constexpr auto kTimer10        = sjsu::ResourceID::Define<kAPB2 + 20>();
static constexpr auto kTimer11        = sjsu::ResourceID::Define<kAPB2 + 21>();
static constexpr auto kCpu            = sjsu::ResourceID::Define<kBeyond + 0>();
static constexpr auto kSystemTimer    = sjsu::ResourceID::Define<kBeyond + 1>();
static constexpr auto kI2s            = sjsu::ResourceID::Define<kBeyond + 2>();
//! @endcond
}  // namespace sjsu::stm32f10x
