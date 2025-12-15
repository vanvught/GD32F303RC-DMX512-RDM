/**
 * @file rdmdevicecontroller.h
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

#ifndef RDMDEVICECONTROLLER_H_
#define RDMDEVICECONTROLLER_H_

#include <algorithm>
#include <cstring>

#include "hal_serialnumber.h"
#include "rdmconst.h"
 #include "firmware/debug/debug_debug.h"

class RDMDeviceController
{
   public:
    RDMDeviceController()
    {
        DEBUG_ENTRY();

        hal::SerialNumber(serial_number_);

        uid_[0] = RDMConst::MANUFACTURER_ID[0];
        uid_[1] = RDMConst::MANUFACTURER_ID[1];
        uid_[2] = serial_number_[0];
        uid_[3] = serial_number_[1];
        uid_[4] = serial_number_[2];
        uid_[5] = serial_number_[3];

        DEBUG_EXIT();
    }

    ~RDMDeviceController() = default;

    const uint8_t* GetUID() const { return uid_; }

    const uint8_t* GetSN() const { return serial_number_; }

    void Print()
    {
        puts("RDM Device configuration");
        const auto kLength = static_cast<int>(std::min(static_cast<size_t>(RDM_MANUFACTURER_LABEL_MAX_LENGTH), strlen(RDMConst::MANUFACTURER_NAME)));
        printf(" Manufacturer Name : %.*s\n", kLength, const_cast<char*>(&RDMConst::MANUFACTURER_NAME[0]));
        printf(" Manufacturer ID   : %.2X%.2X\n", uid_[0], uid_[1]);
        printf(" Serial Number     : %.2X%.2X%.2X%.2X\n", serial_number_[3], serial_number_[2], serial_number_[1], serial_number_[0]);
    }

   private:
    uint8_t uid_[RDM_UID_SIZE];
#define DEVICE_SN_LENGTH 4
    static_assert(DEVICE_SN_LENGTH == hal::kSnSize);
    uint8_t serial_number_[DEVICE_SN_LENGTH];
};

#endif  // RDMDEVICECONTROLLER_H_
