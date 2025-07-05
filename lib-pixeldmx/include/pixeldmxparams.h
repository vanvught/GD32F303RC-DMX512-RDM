#pragma once
/**
 * @file pixeldmxparams.h
 *
 */
/* Copyright (C) 2017-2025 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"); to deal
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

#include "configurationstore.h"

#if !defined(CONFIG_DMXNODE_PIXEL_MAX_PORTS)
#error CONFIG_DMXNODE_PIXEL_MAX_PORTS is not defined
#endif

namespace pixeldmxparams
{
inline constexpr uint32_t kMaxPorts = CONFIG_DMXNODE_PIXEL_MAX_PORTS;

struct Mask
{
    static constexpr uint32_t kType = (1U << 0);
    static constexpr uint32_t kCount = (1U << 1);
    static constexpr uint32_t kDmxStartAddress = (1U << 2);
    static constexpr uint32_t kTestPattern = (1U << 3);
    static constexpr uint32_t kSpiSpeed = (1U << 4);
    static constexpr uint32_t kGlobalBrightness = (1U << 5);
    static constexpr uint32_t kActiveOut = (1U << 6);
    static constexpr uint32_t kGammaCorrection = (1U << 7);
    static constexpr uint32_t kGroupingCount = (1U << 8);
    static constexpr uint32_t kMap = (1U << 9);
    static constexpr uint32_t kLowCode = (1U << 10);
    static constexpr uint32_t kHighCode = (1U << 11);
    static constexpr uint32_t kStartUniPort1 = (1U << 12);
};
} // namespace pixeldmxparams

class PixelDmxParams
{
   public:
    PixelDmxParams() = default;

    PixelDmxParams(const PixelDmxParams&) = delete;
    PixelDmxParams& operator=(const PixelDmxParams&) = delete;

    PixelDmxParams(PixelDmxParams&&) = delete;
    PixelDmxParams& operator=(PixelDmxParams&&) = delete;

    ~PixelDmxParams() = default;

    void Load();
    void Load(const char* buffer, uint32_t length);
    void Builder(char* buffer, uint32_t length, uint32_t& size);
    void Set();

    uint16_t GetStartUniversePort(uint32_t output_port_index, bool& is_set) const
    {
        if (output_port_index < pixeldmxparams::kMaxPorts)
        {
            is_set = IsMaskSet(pixeldmxparams::Mask::kStartUniPort1 << output_port_index);
            return store_dmxled_.start_universe[output_port_index];
        }

        is_set = false;
        return 0;
    }

    uint8_t GetTestPattern() const { return store_dmxled_.test_pattern; }

    static void StaticCallbackFunction(void* p, const char* s);

   private:
    void Dump();
    void CallbackFunction(const char* line);
    bool IsMaskSet(uint32_t mask) const { return (store_dmxled_.set_list & mask) == mask; }

   private:
    common::store::DmxLed store_dmxled_;
};
