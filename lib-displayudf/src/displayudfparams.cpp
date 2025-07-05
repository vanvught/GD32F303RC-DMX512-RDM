/**
 * @file displayudfparams.cpp
 *
 */
/* Copyright (C) 2019-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#if defined(DEBUG_DISPLAYUDF)
#undef NDEBUG
#endif

#include <cstdint>
#include <cstring>
#ifndef NDEBUG
#include <cstdio>
#endif
#include <cassert>

#if defined(NODE_ARTNET_MULTI)
#define NODE_ARTNET
#endif

#if defined(NODE_E131_MULTI)
#define NODE_E131
#endif

#include "displayudf.h"
#include "displayudfparams.h"
#include "displayudfparamsconst.h"

#include "configstore.h"
#include "configurationstore.h"

#include "networkparamsconst.h"
#include "dmxnodeparamsconst.h"

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

#include "display.h"

#if defined(NODE_NODE)
#include "node.h"
#include "nodeparamsconst.h"
#endif
#if defined(NODE_ARTNET)
#include "artnetnode.h"
#endif
#if defined(NODE_E131)
#include "e131bridge.h"
#endif

#include "debug.h"

static const char* array[static_cast<uint32_t>(displayudf::Labels::UNKNOWN)] = {
    DisplayUdfParamsConst::TITLE,
    DisplayUdfParamsConst::BOARD_NAME,
    DisplayUdfParamsConst::VERSION,
    NetworkParamsConst::HOSTNAME,
    NetworkParamsConst::IP_ADDRESS,
    NetworkParamsConst::NET_MASK,
    NetworkParamsConst::DEFAULT_GATEWAY,
    DisplayUdfParamsConst::ACTIVE_PORTS,
    DisplayUdfParamsConst::DMX_DIRECTION,
    DmxNodeParamsConst::DMX_START_ADDRESS,
    DmxNodeParamsConst::UNIVERSE_PORT[0],
#if (DMX_MAX_PORTS > 1)
    DmxNodeParamsConst::UNIVERSE_PORT[1],
#endif
#if (DMX_MAX_PORTS > 2)
    DmxNodeParamsConst::UNIVERSE_PORT[2],
#endif
#if (DMX_MAX_PORTS == 4)
    DmxNodeParamsConst::UNIVERSE_PORT[3],
#endif
#if defined(NODE_ARTNET) && defined(ARTNET_HAVE_DMXIN)
    DmxNodeParamsConst::DESTINATION_IP_PORT[0],
#if DMX_MAX_PORTS >= 2
    DmxNodeParamsConst::DESTINATION_IP_PORT[1],
#endif
#if DMX_MAX_PORTS >= 3
    DmxNodeParamsConst::DESTINATION_IP_PORT[2],
#endif
#if DMX_MAX_PORTS == 4
    DmxNodeParamsConst::DESTINATION_IP_PORT[3],
#endif
#endif
};

static void Update(const common::store::DisplayUdf* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::display_udf);
}


static void Copy(struct ::common::store::DisplayUdf* store)
{
	 ConfigStore::Instance().Copy(store, &ConfigurationStore::display_udf);
}

void DisplayUdfParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(DisplayUdfParams::StaticCallbackFunction, this);

    if (configfile.Read(DisplayUdfParamsConst::FILE_NAME))
    {
        Update(&store_display_udf_);
    }
    else
#endif
    {
        Copy(&store_display_udf_);
    }
#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void DisplayUdfParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    memset(&store_display_udf_, 0, sizeof(store_display_udf_));

    ReadConfigFile config(DisplayUdfParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_display_udf_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void DisplayUdfParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);
    uint8_t value8;

    if (Sscan::Uint8(line, DisplayUdfParamsConst::INTENSITY, value8) == Sscan::OK)
    {
        store_display_udf_.intensity = value8;
        store_display_udf_.set_list |= displayudfparams::Mask::kIntensity;
        return;
    }

    if (Sscan::Uint8(line, DisplayUdfParamsConst::kSleepTimeout, value8) == Sscan::OK)
    {
        store_display_udf_.sleep_timeout = value8;

        if (value8 != display::Defaults::kSleepTimeout)
        {
            store_display_udf_.set_list |= displayudfparams::Mask::kSleepTimeout;
        }
        else
        {
            store_display_udf_.set_list &= ~displayudfparams::Mask::kSleepTimeout;
        }
        return;
    }

    if (Sscan::Uint8(line, DisplayUdfParamsConst::FLIP_VERTICALLY, value8) == Sscan::OK)
    {
        if (value8 != 0)
        {
            store_display_udf_.set_list |= displayudfparams::Mask::kFlipVertically;
        }
        else
        {
            store_display_udf_.set_list &= ~displayudfparams::Mask::kFlipVertically;
        }
        return;
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(displayudf::Labels::UNKNOWN); i++)
    {
        if (Sscan::Uint8(line, array[i], value8) == Sscan::OK)
        {
            if ((value8 > 0) && (value8 <= displayudf::LABEL_MAX_ROWS))
            {
                store_display_udf_.label_index[i] = value8;
                store_display_udf_.set_list |= (1U << i);
            }
            else
            {
                store_display_udf_.label_index[i] = 0;
                store_display_udf_.set_list &= ~(1U << i);
            }
            return;
        }
    }
}

void DisplayUdfParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    assert(buffer != nullptr);

    Copy(&store_display_udf_);

    PropertiesBuilder builder(DisplayUdfParamsConst::FILE_NAME, buffer, length);

    if (!IsMaskSet(displayudfparams::Mask::kIntensity))
    {
        store_display_udf_.intensity = displayudf::defaults::INTENSITY;
    }

    builder.Add(DisplayUdfParamsConst::INTENSITY, store_display_udf_.intensity);

    if (!IsMaskSet(displayudfparams::Mask::kSleepTimeout))
    {
        store_display_udf_.sleep_timeout = display::Defaults::kSleepTimeout;
    }

    builder.Add(DisplayUdfParamsConst::kSleepTimeout, store_display_udf_.sleep_timeout);

    builder.Add(DisplayUdfParamsConst::FLIP_VERTICALLY, IsMaskSet(displayudfparams::Mask::kFlipVertically));

    for (uint32_t i = 0; i < static_cast<uint32_t>(displayudf::Labels::UNKNOWN); i++)
    {
        if (array[i][0] != '\0')
        {
            builder.Add(array[i], store_display_udf_.label_index[i], IsMaskSet(1U << i));
        }
    }

    size = builder.GetSize();
}

void DisplayUdfParams::Set()
{
    auto* display_udf = DisplayUdf::Get();

    if (IsMaskSet(displayudfparams::Mask::kIntensity))
    {
        display_udf->SetContrast(store_display_udf_.intensity);
    }

    display_udf->SetSleepTimeout(store_display_udf_.sleep_timeout);
    display_udf->SetFlipVertically(IsMaskSet(displayudfparams::Mask::kFlipVertically));

    for (uint32_t i = 0; i < static_cast<uint32_t>(displayudf::Labels::UNKNOWN); i++)
    {
        if (IsMaskSet(1U << i))
        {
            display_udf->Set(store_display_udf_.label_index[i], static_cast<displayudf::Labels>(i));
        }
    }
}

void DisplayUdfParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<DisplayUdfParams*>(p))->CallbackFunction(s);
}

void DisplayUdfParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, DisplayUdfParamsConst::FILE_NAME);

    printf(" %s=%d\n", DisplayUdfParamsConst::INTENSITY, store_display_udf_.intensity);
    printf(" %s=%d\n", DisplayUdfParamsConst::kSleepTimeout, store_display_udf_.sleep_timeout);

    if (IsMaskSet(displayudfparams::Mask::kFlipVertically))
    {
        printf(" Flip vertically\n");
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(displayudf::Labels::UNKNOWN); i++)
    {
        if (IsMaskSet(1U << i))
        {
            printf(" %s=%d\n", array[i], store_display_udf_.label_index[i]);
        }
    }
}
