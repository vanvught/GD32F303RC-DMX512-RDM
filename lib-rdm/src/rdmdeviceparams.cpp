/**
 * @file rdmdeviceparams.cpp
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
#include <cstring>
#include <cstdio>
#include <cassert>

#include "rdmdeviceparams.h"
#include "rdmdeviceparamsconst.h"
#include "rdmdevice.h"
#include "rdm_e120.h"

#include "configstore.h"
#include "configurationstore.h"

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

#include "debug.h"

static void Update(const common::store::RdmDevice* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::rdm_device);
}

static void Copy(struct ::common::store::RdmDevice* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::rdm_device);
}

RDMDeviceParams::RDMDeviceParams()
{
    DEBUG_ENTRY

    memset(&store_rdm_device_, 0, sizeof(struct common::store::RdmDevice));

    store_rdm_device_.product_category = E120_PRODUCT_CATEGORY_OTHER;
    store_rdm_device_.product_detail = E120_PRODUCT_DETAIL_OTHER;

    DEBUG_EXIT
}

void RDMDeviceParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    store_rdm_device_.set_list = 0;

    ReadConfigFile configfile(RDMDeviceParams::StaticCallbackFunction, this);

    if (configfile.Read(RDMDeviceParamsConst::FILE_NAME))
    {
        Update(&store_rdm_device_);
    }
    else
#endif
    {
        Copy(&store_rdm_device_);
    }
#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMDeviceParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    store_rdm_device_.set_list = 0;

    ReadConfigFile config(RDMDeviceParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_rdm_device_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMDeviceParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);

    uint32_t length = RDM_DEVICE_LABEL_MAX_LENGTH;

    if (Sscan::Char(line, RDMDeviceParamsConst::LABEL, reinterpret_cast<char *>(store_rdm_device_.device_root_label), length) == Sscan::OK)
    {
        store_rdm_device_.device_root_label_length = static_cast<uint8_t>(length);
        store_rdm_device_.set_list |= rdm::deviceparams::Mask::kLabel;

        return;
    }

    uint16_t value16;

    if (Sscan::HexUint16(line, RDMDeviceParamsConst::PRODUCT_CATEGORY, value16) == Sscan::OK)
    {
        store_rdm_device_.product_category = value16;

        if (value16 == E120_PRODUCT_CATEGORY_OTHER)
        {
            store_rdm_device_.set_list &= ~rdm::deviceparams::Mask::kProductCategory;
        }
        else
        {
            store_rdm_device_.set_list |= rdm::deviceparams::Mask::kProductCategory;
        }

        return;
    }

    if (Sscan::HexUint16(line, RDMDeviceParamsConst::PRODUCT_DETAIL, value16) == Sscan::OK)
    {
        store_rdm_device_.product_detail = value16;

        if (value16 == E120_PRODUCT_DETAIL_OTHER)
        {
            store_rdm_device_.set_list &= ~rdm::deviceparams::Mask::kProductDetail;
        }
        else
        {
            store_rdm_device_.set_list |= rdm::deviceparams::Mask::kProductDetail;
        }

        return;
    }
}

void RDMDeviceParams::Set(RDMDevice *rdm_device)
{
    assert(rdm_device != nullptr);

    TRDMDeviceInfoData info;

    if (IsMaskSet(rdm::deviceparams::Mask::kLabel))
    {
        info.data = reinterpret_cast<char *>(store_rdm_device_.device_root_label);
        info.length = store_rdm_device_.device_root_label_length;
        rdm_device->SetLabel(&info);
    }

    if (IsMaskSet(rdm::deviceparams::Mask::kProductCategory))
    {
        rdm_device->SetProductCategory(store_rdm_device_.product_category);
    }

    if (IsMaskSet(rdm::deviceparams::Mask::kProductDetail))
    {
        rdm_device->SetProductDetail(store_rdm_device_.product_detail);
    }
}

void RDMDeviceParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);

    Copy(&store_rdm_device_);

    PropertiesBuilder builder(RDMDeviceParamsConst::FILE_NAME, buffer, length);

    const auto kIsProductCategory = IsMaskSet(rdm::deviceparams::Mask::kProductCategory);

    if (!kIsProductCategory)
    {
        store_rdm_device_.product_category = RDMDevice::Get()->GetProductCategory();
    }

    builder.AddHex16(RDMDeviceParamsConst::PRODUCT_CATEGORY, store_rdm_device_.product_category, kIsProductCategory);

    const auto kIsProductDetail = IsMaskSet(rdm::deviceparams::Mask::kProductDetail);

    if (!kIsProductDetail)
    {
        store_rdm_device_.product_detail = RDMDevice::Get()->GetProductDetail();
    }

    builder.AddHex16(RDMDeviceParamsConst::PRODUCT_DETAIL, store_rdm_device_.product_detail, kIsProductDetail);

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
    DEBUG_EXIT
}

void RDMDeviceParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<RDMDeviceParams*>(p))->CallbackFunction(s);
}

void RDMDeviceParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, RDMDeviceParamsConst::FILE_NAME);
    printf(" %s=%.*s\n", RDMDeviceParamsConst::LABEL, store_rdm_device_.device_root_label_length, store_rdm_device_.device_root_label);
    printf(" %s=%.4x\n", RDMDeviceParamsConst::PRODUCT_CATEGORY, store_rdm_device_.product_category);
    printf(" %s=%.4x\n", RDMDeviceParamsConst::PRODUCT_DETAIL, store_rdm_device_.product_detail);
}
