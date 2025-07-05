#pragma once
/**
 * @file rdmsubdevicesparams.h
 *
 */
/* Copyright (C) 2020-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "configurationstore.h"

class RDMSubDevice;

class RDMSubDevicesParams
{
   public:
    RDMSubDevicesParams();

    RDMSubDevicesParams(const RDMSubDevicesParams&) = delete;
    RDMSubDevicesParams& operator=(const RDMSubDevicesParams&) = delete;

    RDMSubDevicesParams(RDMSubDevicesParams&&) = delete;
    RDMSubDevicesParams& operator=(RDMSubDevicesParams&&) = delete;

    void Load();
    void Load(const char* buffer, uint32_t length);
    void Builder(char* buffer, uint32_t length, uint32_t& size);
    void Set();

    static void StaticCallbackFunction(void* p, const char* s);

   private:
    void Dump();
    void CallbackFunction(const char* line);
    bool Add(RDMSubDevice* rdm_sub_device);

   private:
    common::store::RdmSubdevices store_rdm_subdevices_;
};
