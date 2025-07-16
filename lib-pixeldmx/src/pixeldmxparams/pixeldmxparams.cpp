/**
 * @file pixeldmxparams.cpp
 */
/* Copyright (C) 2016-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#if defined(DEBUG_PIXELDMX)
#undef NDEBUG
#endif

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cassert>

#include "pixeldmxparams.h"
#include "pixeldmxparamsconst.h"
#include "pixeldmxconfiguration.h"
#include "pixeltype.h"
#include "pixelpatterns.h"
#include "pixelconfiguration.h"

#include "configstore.h"
#include "configurationstore.h"

#if !defined(OUTPUT_DMX_PIXEL_MULTI)
#include "dmxnodeparamsconst.h"
#endif

#if defined(CONFIG_PIXELDMX_ENABLE_GAMMATABLE)
#include "gamma/gamma_tables.h"
#endif

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

#include "dmxledparamsconst.h"

#include "debug.h"

static void Update(const common::store::DmxLed* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::dmx_led);
}

static void Copy(struct ::common::store::DmxLed* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::dmx_led);
}

void PixelDmxParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(PixelDmxParams::StaticCallbackFunction, this);

    if (configfile.Read(DmxLedParamsConst::FILE_NAME))
    {
        Update(&store_dmxled_);
    }
    else
#endif
    {
        Copy(&store_dmxled_);
    }
#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void PixelDmxParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    memset(&store_dmxled_, 0, sizeof(store_dmxled_));

    ReadConfigFile config(PixelDmxParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_dmxled_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void PixelDmxParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);

    char buffer[16];

    uint32_t length = pixel::TYPES_MAX_NAME_LENGTH;

    if (Sscan::Char(line, DmxLedParamsConst::TYPE, buffer, length) == Sscan::OK)
    {
        buffer[length] = '\0';
        const auto kType = pixel::GetType(buffer);

        if (kType != pixel::Type::UNDEFINED)
        {
            store_dmxled_.type = static_cast<uint8_t>(kType);
            store_dmxled_.flags |= pixeldmxparams::Mask::kType;
        }

        return;
    }

    uint16_t value16;

    if (Sscan::Uint16(line, DmxLedParamsConst::COUNT, value16) == Sscan::OK)
    {
        if (value16 != 0 && value16 <= std::max(pixel::max::ledcount::RGB, pixel::max::ledcount::RGBW))
        {
            store_dmxled_.count = value16;
            store_dmxled_.flags |= pixeldmxparams::Mask::kCount;
        }

        return;
    }

    length = 3;

    if (Sscan::Char(line, DmxLedParamsConst::MAP, buffer, length) == Sscan::OK)
    {
        buffer[length] = '\0';

        const auto kMap = pixel::GetMap(buffer);

        if (kMap != pixel::Map::UNDEFINED)
        {
            store_dmxled_.map = static_cast<uint8_t>(kMap);
            store_dmxled_.flags |= pixeldmxparams::Mask::kMap;
        }

        return;
    }

    float value_float;
    uint8_t value8;

    if (Sscan::Float(line, DmxLedParamsConst::LED_T0H, value_float) == Sscan::OK)
    {
        if ((value8 = pixel::ConvertTxH(value_float)) != 0)
        {
            store_dmxled_.low_code = value8;
            store_dmxled_.flags |= pixeldmxparams::Mask::kLowCode;
        }

        return;
    }

    if (Sscan::Float(line, DmxLedParamsConst::LED_T1H, value_float) == Sscan::OK)
    {
        if ((value8 = pixel::ConvertTxH(value_float)) != 0)
        {
            store_dmxled_.high_code = value8;
            store_dmxled_.flags |= pixeldmxparams::Mask::kHighCode;
        }

        return;
    }

    if (Sscan::Uint16(line, DmxLedParamsConst::GROUPING_COUNT, value16) == Sscan::OK)
    {
        if (value16 > 1 && value16 <= std::max(pixel::max::ledcount::RGB, pixel::max::ledcount::RGBW))
        {
            store_dmxled_.grouping_count = value16;
            store_dmxled_.flags |= pixeldmxparams::Mask::kGroupingCount;
        }

        return;
    }

    uint32_t value32;

    if (Sscan::Uint32(line, DmxLedParamsConst::SPI_SPEED_HZ, value32) == Sscan::OK)
    {
        if (value32 != pixel::spi::speed::ws2801::default_hz)
        {
            store_dmxled_.spi_speed_hz = value32;
            store_dmxled_.flags |= pixeldmxparams::Mask::kSpiSpeed;
        }

        return;
    }

    if (Sscan::Uint8(line, DmxLedParamsConst::GLOBAL_BRIGHTNESS, value8) == Sscan::OK)
    {
        if ((value8 != 0) && (value8 != 0xFF))
        {
            store_dmxled_.flags |= pixeldmxparams::Mask::kGlobalBrightness;
            store_dmxled_.global_brightness = value8;
        }

        return;
    }

#if !defined(OUTPUT_DMX_PIXEL_MULTI)
    if (Sscan::Uint16(line, DmxNodeParamsConst::DMX_START_ADDRESS, value16) == Sscan::OK)
    {
        if ((value16 != 0) && (value16 <= dmxnode::kUniverseSize))
        {
            store_dmxled_.dmx_start_address = value16;
            store_dmxled_.flags |= pixeldmxparams::Mask::kDmxStartAddress;
        }

        return;
    }
#endif

    for (uint32_t i = 0; i < pixeldmxparams::kMaxPorts; i++)
    {
        if (Sscan::Uint16(line, PixelDmxParamsConst::START_UNI_PORT[i], value16) == Sscan::OK)
        {
            if (value16 > 0)
            {
                store_dmxled_.start_universe[i] = value16;
                store_dmxled_.flags |= (pixeldmxparams::Mask::kStartUniPort1 << i);
            }
            else
            {
                store_dmxled_.start_universe[i] = static_cast<uint16_t>(1 + (i * 4));
                store_dmxled_.flags &= ~(pixeldmxparams::Mask::kStartUniPort1 << i);
            }
        }
    }

#if defined(OUTPUT_DMX_PIXEL_MULTI)
    if (Sscan::Uint8(line, DmxLedParamsConst::ACTIVE_OUT, value8) == Sscan::OK)
    {
        if ((value8 > 0) && (value8 <= pixeldmxparams::kMaxPorts) && (value8 != pixel::defaults::OUTPUT_PORTS))
        {
            store_dmxled_.active_outputs = value8;
            store_dmxled_.flags |= pixeldmxparams::Mask::kActiveOut;
        }

        return;
    }
#endif

    if (Sscan::Uint8(line, DmxLedParamsConst::TEST_PATTERN, value8) == Sscan::OK)
    {
        if (value8 < static_cast<uint8_t>(pixelpatterns::Pattern::kLast))
        {
            store_dmxled_.test_pattern = value8;
            store_dmxled_.flags |= pixeldmxparams::Mask::kTestPattern;
        }

        return;
    }

#if defined(CONFIG_PIXELDMX_ENABLE_GAMMATABLE)
    if (Sscan::Uint8(line, DmxLedParamsConst::GAMMA_CORRECTION, nValue8) == Sscan::OK)
    {
        if (nValue8 != 0)
        {
            store_dmxled_.flags |= pixeldmxparams::Mask::GAMMA_CORRECTION;
        }
        else
        {
            store_dmxled_.flags &= ~pixeldmxparams::Mask::GAMMA_CORRECTION;
        }
        return;
    }

    if (Sscan::Float(line, DmxLedParamsConst::GAMMA_VALUE, fValue) == Sscan::OK)
    {
        const auto nValue = static_cast<uint8_t>(fValue * 10);
        if ((nValue < gamma::kMin) || (nValue > gamma::kMax))
        {
            store_dmxled_.gamma_value = 0;
        }
        else
        {
            store_dmxled_.gamma_value = nValue;
        }
        return;
    }
#endif
}

void PixelDmxParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    assert(buffer != nullptr);

    auto& pixel_dmx_configuration = PixelDmxConfiguration::Get();

    Copy(&store_dmxled_);

    PropertiesBuilder builder(DmxLedParamsConst::FILE_NAME, buffer, length);

    if (!IsMaskSet(pixeldmxparams::Mask::kType))
    {
        store_dmxled_.type = static_cast<uint8_t>(pixel_dmx_configuration.GetType());
    }

    builder.Add(DmxLedParamsConst::TYPE, pixel::GetType(static_cast<pixel::Type>(store_dmxled_.type)));

    if (store_dmxled_.count == 0)
    {
        store_dmxled_.count = pixel_dmx_configuration.GetCount();
    }

    builder.Add(DmxLedParamsConst::COUNT, store_dmxled_.count);

#if defined(CONFIG_PIXELDMX_ENABLE_GAMMATABLE)
    builder.Add(DmxLedParamsConst::GAMMA_CORRECTION, IsMaskSet(pixeldmxparams::Mask::GAMMA_CORRECTION));

    if (store_dmxled_.gamma_value == 0)
    {
        builder.Add(DmxLedParamsConst::GAMMA_VALUE, "<default>", false);
    }
    else
    {
        builder.Add(DmxLedParamsConst::GAMMA_VALUE, static_cast<float>(store_dmxled_.gamma_value) / 10, true);
    }
#endif

    if (!IsMaskSet(pixeldmxparams::Mask::kMap))
    {
        store_dmxled_.map = static_cast<uint8_t>(pixel_dmx_configuration.GetMap());
    }

    builder.AddComment("Overwrite datasheet");
    builder.Add(DmxLedParamsConst::MAP, pixel::GetMap(static_cast<pixel::Map>(store_dmxled_.map)));

    if (!IsMaskSet(pixeldmxparams::Mask::kLowCode) || !IsMaskSet(pixeldmxparams::Mask::kHighCode))
    {
        uint8_t low_code;
        uint8_t high_code;

        PixelConfiguration::Get().GetTxH(static_cast<pixel::Type>(store_dmxled_.type), low_code, high_code);

        if (!IsMaskSet(pixeldmxparams::Mask::kLowCode))
        {
            store_dmxled_.low_code = low_code;
        }

        if (!IsMaskSet(pixeldmxparams::Mask::kHighCode))
        {
            store_dmxled_.high_code = high_code;
        }
    }

    builder.AddComment("Overwrite timing (us)");
    builder.Add(DmxLedParamsConst::LED_T0H, pixel::ConvertTxH(store_dmxled_.low_code), true, 2);
    builder.Add(DmxLedParamsConst::LED_T1H, pixel::ConvertTxH(store_dmxled_.high_code), true, 2);

    if (store_dmxled_.grouping_count == 0)
    {
        store_dmxled_.grouping_count = pixel_dmx_configuration.GetGroupingCount();
    }

    builder.AddComment("Grouping");
    builder.Add(DmxLedParamsConst::GROUPING_COUNT, store_dmxled_.grouping_count);

    if (store_dmxled_.spi_speed_hz == 0)
    {
        store_dmxled_.spi_speed_hz = pixel_dmx_configuration.GetClockSpeedHz();
    }

    builder.AddComment("Clock based chips");
    builder.Add(DmxLedParamsConst::SPI_SPEED_HZ, store_dmxled_.spi_speed_hz);

    if (store_dmxled_.global_brightness == 0)
    {
        store_dmxled_.global_brightness = pixel_dmx_configuration.GetGlobalBrightness();
    }

    builder.AddComment("APA102/SK9822");
    builder.Add(DmxLedParamsConst::GLOBAL_BRIGHTNESS, store_dmxled_.global_brightness);

#if !defined(OUTPUT_DMX_PIXEL_MULTI)
    if (store_dmxled_.dmx_start_address == 0)
    {
        store_dmxled_.dmx_start_address = pixel_dmx_configuration.GetDmxStartAddress();
    }

    builder.AddComment("DMX");
    builder.Add(DmxNodeParamsConst::DMX_START_ADDRESS, store_dmxled_.dmx_start_address);
#endif

    for (uint32_t i = 0; i < pixeldmxparams::kMaxPorts; i++)
    {
        builder.Add(PixelDmxParamsConst::START_UNI_PORT[i], store_dmxled_.start_universe[i], IsMaskSet(pixeldmxparams::Mask::kStartUniPort1 << i));
    }

#if defined(OUTPUT_DMX_PIXEL_MULTI)
    if (store_dmxled_.active_outputs == 0)
    {
        store_dmxled_.active_outputs = pixel_dmx_configuration.GetOutputPorts();
    }

    builder.Add(DmxLedParamsConst::ACTIVE_OUT, store_dmxled_.active_outputs);
#endif

    builder.AddComment("Test pattern");
    builder.Add(DmxLedParamsConst::TEST_PATTERN, store_dmxled_.test_pattern);

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
}

void PixelDmxParams::Set()
{
    // Pixel
    auto& pixel_configuration = PixelConfiguration::Get();

    if (IsMaskSet(pixeldmxparams::Mask::kType))
    {
        pixel_configuration.SetType(static_cast<pixel::Type>(store_dmxled_.type));
    }

    if (IsMaskSet(pixeldmxparams::Mask::kCount))
    {
        pixel_configuration.SetCount(store_dmxled_.count);
    }

    if (IsMaskSet(pixeldmxparams::Mask::kMap))
    {
        pixel_configuration.SetMap(static_cast<pixel::Map>(store_dmxled_.map));
    }

    if (IsMaskSet(pixeldmxparams::Mask::kLowCode))
    {
        pixel_configuration.SetLowCode(store_dmxled_.low_code);
    }

    if (IsMaskSet(pixeldmxparams::Mask::kHighCode))
    {
        pixel_configuration.SetHighCode(store_dmxled_.high_code);
    }

    if (IsMaskSet(pixeldmxparams::Mask::kSpiSpeed))
    {
        pixel_configuration.SetClockSpeedHz(store_dmxled_.spi_speed_hz);
    }

    if (IsMaskSet(pixeldmxparams::Mask::kGlobalBrightness))
    {
        pixel_configuration.SetGlobalBrightness(store_dmxled_.global_brightness);
    }

#if defined(CONFIG_PIXELDMX_ENABLE_GAMMATABLE)
    if (IsMaskSet(pixeldmxparams::Mask::GAMMA_CORRECTION))
    {
        pixelConfiguration.SetEnableGammaCorrection(true);
        if (store_dmxled_.gamma_value != 0)
        {
            pixelConfiguration.SetGammaTable(store_dmxled_.gamma_value);
        }
    }
#endif

    // Dmx
    auto& pixel_dmx_configuration = PixelDmxConfiguration::Get();

#if !defined(OUTPUT_DMX_PIXEL_MULTI)
    if (IsMaskSet(pixeldmxparams::Mask::kDmxStartAddress))
    {
        pixel_dmx_configuration.SetDmxStartAddress(store_dmxled_.dmx_start_address);
    }
#endif

    if (IsMaskSet(pixeldmxparams::Mask::kGroupingCount))
    {
        pixel_dmx_configuration.SetGroupingCount(store_dmxled_.grouping_count);
    }

#if defined(OUTPUT_DMX_PIXEL_MULTI)
    if (IsMaskSet(pixeldmxparams::Mask::kActiveOut))
    {
        pixel_dmx_configuration.SetOutputPorts(store_dmxled_.active_outputs);
    }
#endif
}

void PixelDmxParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<PixelDmxParams*>(p))->CallbackFunction(s);
}

void PixelDmxParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, DmxLedParamsConst::FILE_NAME);
    printf(" %s=%s [%d]\n", DmxLedParamsConst::TYPE, pixel::GetType(static_cast<pixel::Type>(store_dmxled_.type)), static_cast<int>(store_dmxled_.type));
    printf(" %s=%d [%s]\n", DmxLedParamsConst::MAP, static_cast<int>(store_dmxled_.map), pixel::GetMap(static_cast<pixel::Map>(store_dmxled_.map)));
    printf(" %s=%.2f [0x%X]\n", DmxLedParamsConst::LED_T0H, pixel::ConvertTxH(store_dmxled_.low_code), store_dmxled_.low_code);
    printf(" %s=%.2f [0x%X]\n", DmxLedParamsConst::LED_T1H, pixel::ConvertTxH(store_dmxled_.high_code), store_dmxled_.high_code);
    printf(" %s=%d\n", DmxLedParamsConst::COUNT, store_dmxled_.count);
    printf(" %s=%d\n", DmxLedParamsConst::GROUPING_COUNT, store_dmxled_.grouping_count);

    for (uint32_t i = 0; i < std::min(static_cast<size_t>(pixelpatterns::kMaxPorts),
                                      sizeof(PixelDmxParamsConst::START_UNI_PORT) / sizeof(PixelDmxParamsConst::START_UNI_PORT[0]));
         i++)
    {
        printf(" %s=%d\n", PixelDmxParamsConst::START_UNI_PORT[i], store_dmxled_.start_universe[i]);
    }

    printf(" %s=%d\n", DmxLedParamsConst::ACTIVE_OUT, store_dmxled_.active_outputs);
    printf(" %s=%u\n", DmxLedParamsConst::SPI_SPEED_HZ, static_cast<unsigned int>(store_dmxled_.spi_speed_hz));
    printf(" %s=%d\n", DmxLedParamsConst::GLOBAL_BRIGHTNESS, store_dmxled_.global_brightness);
#if !defined(OUTPUT_DMX_PIXEL_MULTI)
    printf(" %s=%d\n", DmxNodeParamsConst::DMX_START_ADDRESS, store_dmxled_.dmx_start_address);
#endif
    printf(" %s=%d\n", DmxLedParamsConst::TEST_PATTERN, store_dmxled_.test_pattern);
#if defined(CONFIG_PIXELDMX_ENABLE_GAMMATABLE)
    printf(" %s=%d\n", DmxLedParamsConst::GAMMA_CORRECTION, IsMaskSet(pixeldmxparams::Mask::GAMMA_CORRECTION));
    printf(" %s=%1.1f [%u]\n", DmxLedParamsConst::GAMMA_VALUE, static_cast<float>(store_dmxled_.gamma_value) / 10, store_dmxled_.gamma_value);
#endif
}
