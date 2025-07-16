/**
 * @file main.cpp
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

#include <cstdio>
#include <cstdint>

#include "hal.h"
#include "gd32/hal_watchdog.h"
#include "hal_boardinfo.h"
#include "noemac/network.h"
#include "widget.h"
#include "widgetparams.h"
#include "rdmdeviceparams.h"
#include "configstore.h"
#include "software_version.h"

#ifndef ALIGNED
#define ALIGNED __attribute__((aligned(4)))
#endif

static constexpr char kWidgetModeNames[4][12] ALIGNED = {"DMX_RDM", "DMX", "RDM", "RDM_SNIFFER"};
static constexpr TRDMDeviceInfoData kDeviceLabel ALIGNED = {const_cast<char*>("GD32F103RC DMX USB Pro"), 22};

int main()
{
    hal::Init();
    ConfigStore config_store;
    Network nw;

    Widget widget;
    widget.SetPortDirection(0, dmx::PortDirection::kInput, false);

    WidgetParams widget_params;

    widget_params.Load();
    widget_params.Set();

    widget.SetLabel(&kDeviceLabel);

    RDMDeviceParams rdm_device_params;

    rdm_device_params.Load();
    rdm_device_params.Set(&widget);

    widget.Init();

    const auto* rdm_device_uid = widget.GetUID();
    TRDMDeviceInfoData rdm_device_label;
    widget.GetLabel(&rdm_device_label);
    const auto kWidgetMode = widget_params.GetMode();

    uint8_t hw_text_length;
    printf("[V%s] %s Compiled on %s at %s\n", SOFTWARE_VERSION, hal::BoardName(hw_text_length), __DATE__, __TIME__);
    printf("RDM Controller with USB [Compatible with Enttec USB Pro protocol], Widget mode : %d (%s)\n", kWidgetMode, kWidgetModeNames[static_cast<uint32_t>(kWidgetMode)]);
    printf("Device UUID : %.2x%.2x:%.2x%.2x%.2x%.2x, ", rdm_device_uid[0], rdm_device_uid[1], rdm_device_uid[2], rdm_device_uid[3], rdm_device_uid[4], rdm_device_uid[5]);
    printf("Label : %.*s\n", static_cast<int>(rdm_device_label.length), reinterpret_cast<const char*>(rdm_device_label.data));

    hal::WatchdogInit();

    if (kWidgetMode == widget::Mode::kRdmSniffer)
    {
        widget.SetPortDirection(0, dmx::PortDirection::kInput, true);
        widget.SnifferFillTransmitBuffer(); // Prevent missing first frame
    }

    for (;;)
    {
        hal::WatchdogFeed();
        widget.Run();
        hal::Run();
    }
}
