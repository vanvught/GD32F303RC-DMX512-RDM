#pragma once
/**
 * @file pixel.h
 */
/* Copyright (C) 2024-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#pragma GCC push_options
#pragma GCC optimize("O3")
#pragma GCC optimize("no-tree-loop-distribute-patterns")
#pragma GCC optimize("-fprefetch-loop-arrays")

#include <cstdint>
#include <cassert>

#if defined(PIXELPATTERNS_MULTI)
#include "pixeloutputmulti.h"
#else
#include "pixeloutput.h"
#endif
#include "pixeltype.h"
#include "pixelconfiguration.h"

namespace pixel
{
inline uint32_t GetColour(uint8_t red, uint8_t green, uint8_t blue)
{
    return static_cast<uint32_t>(red << 16) | static_cast<uint32_t>(green << 8) | blue;
}

inline uint32_t GetColour(uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
    return static_cast<uint32_t>(white << 24) | static_cast<uint32_t>(red << 16) | static_cast<uint32_t>(green << 8) | blue;
}

inline uint8_t GetWhite(uint32_t colour)
{
    return (colour >> 24) & 0xFF;
}

inline uint8_t GetRed(uint32_t colour)
{
    return (colour >> 16) & 0xFF;
}

inline uint8_t GetGreen(uint32_t colour)
{
    return (colour >> 8) & 0xFF;
}

inline uint8_t GetBlue(uint32_t colour)
{
    return colour & 0xFF;
}

inline void SetPixelColour([[maybe_unused]] uint32_t port_index, uint32_t pixel_index, uint32_t colour)
{
    auto* output_type = OutputType::Get();
    assert(output_type != nullptr);

    const auto kRed =GetRed(colour);
    const auto kGreen = GetGreen(colour);
    const auto kBlue = GetBlue(colour);

#if defined(PIXELPATTERNS_MULTI)
    switch (PixelConfiguration::Get().GetType())
    {
        case pixel::Type::WS2801:
            output_type->SetColourWS2801(port_index, pixel_index, kRed, kGreen, kBlue);
            break;
        case pixel::Type::APA102:
        case pixel::Type::SK9822:
            output_type->SetPixel4Bytes(port_index, pixel_index, 0xFF, kRed, kGreen, kBlue);
            break;
        case pixel::Type::P9813:
        {
            const auto kFlag = static_cast<uint8_t>(0xC0 | ((~kBlue & 0xC0) >> 2) | ((~kRed & 0xC0) >> 4) | ((~kRed & 0xC0) >> 6));
            output_type->SetPixel4Bytes(port_index, pixel_index, kFlag, kBlue, kGreen, kRed);
        }
        break;
        case pixel::Type::SK6812W:
        {
            const auto kWhite = pixel::GetWhite(colour);
            output_type->SetColourRTZ(port_index, pixel_index, kRed, kGreen, kBlue, kWhite);
        }
        break;
        default:
            output_type->SetColourRTZ(port_index, pixel_index, kRed, kGreen, kBlue);
            break;
    }
#else
    auto& pixelConfiguration = PixelConfiguration::Get();
    const auto kType = pixelConfiguration.GetType();

    if (kType != pixel::Type::SK6812W)
    {
        output_type->SetPixel(pixel_index, kRed, kGreen, kBlue);
    }
    else
    {
        if ((kRed == kGreen) && (kGreen == kBlue))
        {
            output_type->SetPixel(pixel_index, 0x00, 0x00, 0x00, kRed);
        }
        else
        {
            output_type->SetPixel(pixel_index, kRed, kGreen, kBlue, 0x00);
        }
    }
#endif
}

inline void SetPixelColour(uint32_t port_index, uint32_t colour)
{
    const auto kCount = PixelConfiguration::Get().GetCount();

    for (uint32_t i = 0; i < kCount; i++)
    {
        SetPixelColour(port_index, i, colour);
    }
}

inline bool IsUpdating()
{
    auto* output_type = OutputType::Get();
    assert(output_type != nullptr);
    return output_type->IsUpdating();
}

inline void Update()
{
    auto* p_output_type = OutputType::Get();
    assert(p_output_type != nullptr);
    return p_output_type->Update();
}
} // namespace pixel

#pragma GCC pop_options
