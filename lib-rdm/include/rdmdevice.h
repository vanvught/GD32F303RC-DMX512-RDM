#pragma once
/**
 * @file rdmdevice.h
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

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "rdmdevicestore.h"
#include "rdmconst.h"
#include "rdm_e120.h"

#include "debug.h"

struct TRDMDeviceInfoData
{
    char* data;
    uint8_t length;
};

class RDMDevice
{
   public:
    RDMDevice();

    void Init()
    {
        assert(!is_init_);
        is_init_ = true;

        RDMDevice::SetFactoryDefaults();
    }

    void Print();

    void SetFactoryDefaults()
    {
        DEBUG_ENTRY
        TRDMDeviceInfoData info = {m_aFactoryRootLabel, m_nFactoryRootLabelLength};

        RDMDevice::SetLabel(&info);

        m_nCheckSum = RDMDevice::CalculateChecksum();
        DEBUG_EXIT
    }

    bool GetFactoryDefaults() { return (m_nCheckSum == RDMDevice::CalculateChecksum()); }

    const uint8_t* GetUID() const
    {
#if defined(NO_EMAC) || !defined(CONFIG_RDMDEVICE_UUID_IP)
#else
        const auto nIp = net::GetPrimaryIp();
#if !defined(CONFIG_RDMDEVICE_REVERSE_UID)
        m_aUID[5] = static_cast<uint8_t>(nIp >> 24);
        m_aUID[4] = (nIp >> 16) & 0xFF;
        m_aUID[3] = (nIp >> 8) & 0xFF;
        m_aUID[2] = nIp & 0xFF;
#else
        m_aUID[2] = static_cast<uint8_t>(nIp >> 24);
        m_aUID[3] = (nIp >> 16) & 0xFF;
        m_aUID[4] = (nIp >> 8) & 0xFF;
        m_aUID[5] = nIp & 0xFF;
#endif
#endif
        return m_aUID;
    }

    const uint8_t* GetSN() const
    {
#if defined(NO_EMAC) || !defined(CONFIG_RDMDEVICE_UUID_IP)
#else
        GetUID();
        m_aSN[0] = m_aUID[5];
        m_aSN[1] = m_aUID[4];
        m_aSN[2] = m_aUID[3];
        m_aSN[3] = m_aUID[2];
#endif
        return m_aSN;
    }

    void GetManufacturerId(struct TRDMDeviceInfoData* info_data)
    {
        info_data->data = reinterpret_cast<char*>(const_cast<uint8_t*>(RDMConst::MANUFACTURER_ID));
        info_data->length = RDM_DEVICE_MANUFACTURER_ID_LENGTH;
    }

    void GetManufacturerName(struct TRDMDeviceInfoData* info_data)
    {
        info_data->data = const_cast<char*>(&RDMConst::MANUFACTURER_NAME[0]);
        info_data->length = static_cast<uint8_t>(std::min(static_cast<size_t>(RDM_MANUFACTURER_LABEL_MAX_LENGTH), strlen(RDMConst::MANUFACTURER_NAME)));
    }

    void SetLabel(const struct TRDMDeviceInfoData* info_data)
    {
        const auto kLength = std::min(static_cast<uint8_t>(RDM_DEVICE_LABEL_MAX_LENGTH), info_data->length);

        if (is_init_)
        {
            memcpy(m_aRootLabel, info_data->data, kLength);
            m_nRootLabelLength = kLength;

            rdmdevice_store::SaveLabel(m_aRootLabel, m_nRootLabelLength);
        }
        else
        {
            memcpy(m_aFactoryRootLabel, info_data->data, kLength);
            m_nFactoryRootLabelLength = kLength;
        }
    }

    void GetLabel(struct TRDMDeviceInfoData* info_data)
    {
        info_data->data = m_aRootLabel;
        info_data->length = m_nRootLabelLength;
    }

    void SetProductCategory(uint16_t product_category) { m_nProductCategory = product_category; }
    uint16_t GetProductCategory() const { return m_nProductCategory; }

    void SetProductDetail(uint16_t product_detail) { m_nProductDetail = product_detail; }
    uint16_t GetProductDetail() const { return m_nProductDetail; }

    static RDMDevice* Get() { return s_this; }

   private:
    uint16_t CalculateChecksum()
    {
        uint16_t checksum = m_nFactoryRootLabelLength;

        for (uint32_t i = 0; i < m_nRootLabelLength; i++)
        {
            checksum = static_cast<uint16_t>(checksum + m_aRootLabel[i]);
        }

        return checksum;
    }

   private:
    char m_aFactoryRootLabel[RDM_DEVICE_LABEL_MAX_LENGTH];
    char m_aRootLabel[RDM_DEVICE_LABEL_MAX_LENGTH];

    uint16_t m_nProductCategory{E120_PRODUCT_CATEGORY_OTHER};
    uint16_t m_nProductDetail{E120_PRODUCT_DETAIL_OTHER};
    uint16_t m_nCheckSum{0};

    uint8_t m_aUID[RDM_UID_SIZE];
#define DEVICE_SN_LENGTH 4
    uint8_t m_aSN[DEVICE_SN_LENGTH];
    uint8_t m_nRootLabelLength{0};
    uint8_t m_nFactoryRootLabelLength{0};

    bool is_init_{false};

    static inline RDMDevice* s_this;
};
