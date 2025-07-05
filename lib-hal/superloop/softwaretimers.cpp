/**
 * @file softwaretimers.cpp
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

#if defined(DEBUG_HAL_TIMERS)
#undef NDEBUG
#endif

#if defined(__GNUC__) && !defined(__clang__)
#if !defined(CONFIG_REMOTECONFIG_MINIMUM)
#pragma GCC push_options
#pragma GCC optimize("O2")
#pragma GCC optimize("no-tree-loop-distribute-patterns")
#endif
#endif

#include <cstdint>

#include "softwaretimers.h"
#include "hal_millis.h"

void ConsoleError(const char*);

struct Timer
{
    uint32_t expire_time;
    uint32_t interval_millis;
    int32_t id;
    TimerCallbackFunction_t callback_function;
};

static Timer s_timers[hal::SOFTWARE_TIMERS_MAX];
static uint32_t s_timers_count;
static int32_t s_next_id;

TimerHandle_t SoftwareTimerAdd(uint32_t interval_millis, const TimerCallbackFunction_t callback_function)
{
    if (s_timers_count >= hal::SOFTWARE_TIMERS_MAX)
    {
#ifndef NDEBUG
        ConsoleError("SoftwareTimerAdd: Max timer limit reached\n");
#endif
        return -1;
    }

    const auto kCurrentTime = hal::Millis();
    // TODO (a) Prevent potential overflow when calculating expiration time.

    Timer new_timer = {
        .expire_time = kCurrentTime + interval_millis,
        .interval_millis = interval_millis,
        .id = s_next_id++,
        .callback_function = callback_function,
    };

    s_timers[s_timers_count++] = new_timer;

    return new_timer.id;
}

bool SoftwareTimerDelete(TimerHandle_t& id)
{
    for (uint32_t i = 0; i < s_timers_count; ++i)
    {
        if (s_timers[i].id == id)
        {
            // Swap with the last timer to efficiently remove the current timer
            s_timers[i] = s_timers[s_timers_count - 1];
            --s_timers_count;
            id = -1;
            return true;
        }
    }

#ifndef NDEBUG
    ConsoleError("SoftwareTimerDelete: Timer not found\n");
#endif

    return false;
}

bool SoftwareTimerChange(TimerHandle_t id, uint32_t nIntervalMillis)
{
    for (uint32_t i = 0; i < s_timers_count; ++i)
    {
        if (s_timers[i].id == id)
        {
            const auto kCurrentTime = hal::Millis();
            s_timers[i].expire_time = kCurrentTime + nIntervalMillis;
            s_timers[i].interval_millis = nIntervalMillis;
            return true;
        }
    }

#ifndef NDEBUG
    ConsoleError("SoftwareTimerChange: Timer not found\n");
#endif

    return false;
}

void SoftwareTimerRun()
{
    const auto kCurrentTime = hal::Millis();

    for (uint32_t i = 0; i < s_timers_count; i++)
    {
        if (s_timers[i].expire_time <= kCurrentTime)
        {
            s_timers[i].callback_function(s_timers[i].id);
            s_timers[i].expire_time = kCurrentTime + s_timers[i].interval_millis;
        }
    }
}
