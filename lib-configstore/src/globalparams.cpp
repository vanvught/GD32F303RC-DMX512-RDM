/**
 * @file globalparams.cpp
 *
 */
/* Copyright (C) 2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#if defined(DEBUG_GLOBAL)
#undef NDEBUG
#endif

#include <cstdint>
#include <cassert>

#include "globalparams.h"
#include "globalparamsconst.h"

#include "utc.h"
#include "utc.h"

#include "readconfigfile.h"
#include "sscan.h"

#include "propertiesbuilder.h"

#include "configstore.h"
#include "configurationstore.h"

#include "debug.h"

static void Update(const common::store::Global* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::global);
}

static void Copy(struct ::common::store::Global* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::global);
}

void GlobalParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(GlobalParams::StaticCallbackFunction, this);

    if (configfile.Read(GlobalParamsConst::FILE_NAME))
    {
        Update(&store_global_);
    }
    else
#endif
    {
        Copy(&store_global_);
    }

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void GlobalParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    ReadConfigFile config(GlobalParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_global_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void GlobalParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);
    DEBUG_PUTS(line);

    int32_t utc_offset_hours;
    uint32_t utc_offset_minutes;

    if (Sscan::UtcOffset(line, GlobalParamsConst::UTC_OFFSET, utc_offset_hours, utc_offset_minutes) == Sscan::OK)
    {
        int32_t utc_offset;
        if (hal::utc::ValidateOffset(utc_offset_hours, utc_offset_minutes, utc_offset))
        {
            store_global_.utc_offset = utc_offset;
            DEBUG_PUTS("utc_offset OK.");
        }
        else
        {
            store_global_.utc_offset = 0;
            DEBUG_PUTS("utc_offset failed.");
        }

        return;
    }
}

void GlobalParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

    Copy(&store_global_);

    assert(buffer != nullptr);

    PropertiesBuilder builder(GlobalParamsConst::FILE_NAME, buffer, length);

    int32_t utc_offset_hours;
    uint32_t utc_offset_minutes;

    hal::utc::SplitOffset(store_global_.utc_offset, utc_offset_hours, utc_offset_minutes);

    builder.AddUtcOffset(GlobalParamsConst::UTC_OFFSET, utc_offset_hours, utc_offset_minutes);

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
    DEBUG_EXIT
}

void GlobalParams::Set()
{
    int32_t utc_offset_hours;
    uint32_t utc_offset_minutes;
    hal::utc::SplitOffset(store_global_.utc_offset, utc_offset_hours, utc_offset_minutes);
    hal::utc::SetOffsetIfValid(utc_offset_hours, utc_offset_minutes);
}

void GlobalParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, GlobalParamsConst::FILE_NAME);
    int32_t utc_offset_hours;
    uint32_t utc_offset_minutes;
    hal::utc::SplitOffset(store_global_.utc_offset, utc_offset_hours, utc_offset_minutes);
    printf("%d [%d:%u]\n", store_global_.utc_offset, utc_offset_hours, utc_offset_minutes);
}
