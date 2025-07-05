#pragma once
/**
 * @file rdmdeviceparams.h
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

#include <cstdint>

#include "configurationstore.h"

namespace rdm::deviceparams
{
struct Mask
{
    static constexpr uint32_t kLabel = (1U << 0);
    static constexpr uint32_t kProductCategory = (1U << 1);
    static constexpr uint32_t kProductDetail = (1U << 2);
};
} // namespace rdm::deviceparams

class RDMDevice;

class RDMDeviceParams
{
   public:
    RDMDeviceParams();

    RDMDeviceParams(const RDMDeviceParams&) = delete;
    RDMDeviceParams& operator=(const RDMDeviceParams&) = delete;

    RDMDeviceParams(RDMDeviceParams&&) = delete;
    RDMDeviceParams& operator=(RDMDeviceParams&&) = delete;

    void Load();
    void Load(const char* buffer, uint32_t length);
    void Builder(char* buffer, uint32_t length, uint32_t& size);
    void Set(RDMDevice *rdm_device);

    static void StaticCallbackFunction(void* p, const char* s);

   private:
    void Dump();
    void CallbackFunction(const char* s);
    bool IsMaskSet(uint32_t mask) const { return (store_rdm_device_.set_list & mask) == mask; }

   private:
    common::store::RdmDevice store_rdm_device_;
};
