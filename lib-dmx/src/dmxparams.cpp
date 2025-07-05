/**
 * @file dmxparams.cpp
 *
 */
/* Copyright (C) 2017-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "configurationstore.h"
#if defined(DEBUG_DMX)
#undef NDEBUG
#endif

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "dmxparams.h"
#include "dmxconst.h"
#include "dmx.h"

#include "configstore.h"
#include "configurationstore.h"

#include "readconfigfile.h"
#include "sscan.h"

#include "propertiesbuilder.h"

#include "debug.h"

struct DmxParamsConst
{
    constexpr static const char kFileName[] = "dmxsend.txt";
    constexpr static const char kBreakTime[] = "break_time";
    constexpr static const char kMabTime[] = "mab_time";
    constexpr static const char kRefreshRate[] = "refresh_rate";
    constexpr static const char kSlotsCount[] = "slots_count";
};

static constexpr uint8_t RounddownSlots(uint16_t n)
{
    return static_cast<uint8_t>((n / 2U) - 1);
}
static constexpr uint16_t RoundupSlots(uint8_t n)
{
    return static_cast<uint16_t>((n + 1U) * 2U);
}

static void Update(const common::store::DmxSend* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::dmx_send);
}

static void Copy(struct ::common::store::DmxSend* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::dmx_send);
}

void DmxParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(DmxParams::StaticCallbackFunction, this);

    if (configfile.Read(DmxParamsConst::kFileName))
    {
        Update(&store_dmx_send_);
    }
    else
#endif
    {
        Copy(&store_dmx_send_);
    }
#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void DmxParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    memset(&store_dmx_send_, 0, sizeof(store_dmx_send_));

    ReadConfigFile config(DmxParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_dmx_send_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void DmxParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);

    uint16_t value16;

    if (Sscan::Uint16(line, DmxParamsConst::kBreakTime, value16) == Sscan::OK)
    {
        if (value16 >= dmx::transmit::BREAK_TIME_MIN)
        {
            store_dmx_send_.break_time = value16;
        }
        return;
    }

    if (Sscan::Uint16(line, DmxParamsConst::kMabTime, value16) == Sscan::OK)
    {
        if (value16 > dmx::transmit::MAB_TIME_MIN)
        {
            store_dmx_send_.mab_time = value16;
        }
        return;
    }

    uint8_t value8;

    if (Sscan::Uint8(line, DmxParamsConst::kRefreshRate, value8) == Sscan::OK)
    {
        store_dmx_send_.refresh_rate = value8;
        return;
    }

    if (Sscan::Uint16(line, DmxParamsConst::kSlotsCount, value16) == Sscan::OK)
    {
        if ((value16 >= 2) && (value16 <= dmx::kChannelsMax))
        {
            store_dmx_send_.slots_count = RounddownSlots(value16);
        }
        return;
    }
}

void DmxParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);

    Copy(&store_dmx_send_);

    PropertiesBuilder builder(DmxParamsConst::kFileName, buffer, length);

    if (store_dmx_send_.break_time == 0)
    {
        store_dmx_send_.break_time = Dmx::Get()->GetDmxBreakTime();
    }

    builder.Add(DmxParamsConst::kBreakTime, store_dmx_send_.break_time);

    if (store_dmx_send_.mab_time == 0)
    {
        store_dmx_send_.mab_time = Dmx::Get()->GetDmxMabTime();
    }

    builder.Add(DmxParamsConst::kMabTime, store_dmx_send_.mab_time);
    builder.Add(DmxParamsConst::kRefreshRate, store_dmx_send_.refresh_rate);

    if (store_dmx_send_.slots_count == 0)
    {
        store_dmx_send_.slots_count = RounddownSlots(Dmx::Get()->GetDmxSlots());
    }

    builder.Add(DmxParamsConst::kSlotsCount, RoundupSlots(store_dmx_send_.slots_count));

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
    DEBUG_EXIT
}

void DmxParams::Set()
{
    auto* dmx = Dmx::Get();

    dmx->SetDmxBreakTime(store_dmx_send_.break_time);
    dmx->SetDmxMabTime(store_dmx_send_.mab_time);

    uint32_t period = 0;
    if (store_dmx_send_.refresh_rate != 0)
    {
        period = 1000000U / store_dmx_send_.refresh_rate;
    }
    dmx->SetDmxPeriodTime(period);

    if (store_dmx_send_.slots_count != 0)
    {
        dmx->SetDmxSlots(RoundupSlots(store_dmx_send_.slots_count));
    }
}

void DmxParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<DmxParams*>(p))->CallbackFunction(s);
}

void DmxParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, DmxParamsConst::kFileName);
    printf(" %s=%d\n", DmxParamsConst::kBreakTime, store_dmx_send_.break_time);
    printf(" %s=%d\n", DmxParamsConst::kMabTime, store_dmx_send_.mab_time);
    printf(" %s=%d\n", DmxParamsConst::kRefreshRate, store_dmx_send_.refresh_rate);
    printf(" %s=%d [%d]\n", DmxParamsConst::kSlotsCount, store_dmx_send_.slots_count, 	RoundupSlots(store_dmx_send_.slots_count));
}
