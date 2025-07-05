#pragma once
/**
 * @file dmxconst.h
 *
 */
/* Copyright (C) 2021-2025 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cstdint>

namespace dmx
{
enum class PortDirection
{
    kInput,
    kOutput,
    kDisable
};

enum class OutputStyle
{
    kDelta,   ///< DMX frame is triggered
    kConstant ///< DMX output is continuous
};

enum class SendStyle
{
    kDirect,
    kSync
};

static constexpr uint32_t kStartCode = 0; ///< The start code for DMX512 data. This is often referred to as NSC for "Null Start Code".
static constexpr uint32_t kChannelsMin = 2;
static constexpr uint32_t kChannelsMax = 512;
namespace transmit
{
static constexpr uint32_t BREAK_TIME_MIN = 92;                                ///< 92 us
static constexpr uint32_t BREAK_TIME_TYPICAL = 176;                           ///< 176 us
static constexpr uint32_t MAB_TIME_MIN = 12;                                  ///< 12 us
static constexpr uint32_t MAB_TIME_MAX = 1000000;                             ///< 1s
static constexpr uint32_t REFRESH_RATE_DEFAULT = 40;                          ///< 40 Hz
static constexpr uint32_t PERIOD_DEFAULT = (1000000U / REFRESH_RATE_DEFAULT); ///< 25000 us
static constexpr uint32_t BREAK_TO_BREAK_TIME_MIN = 1204;                     ///< us
} // namespace transmit
} // namespace dmx
