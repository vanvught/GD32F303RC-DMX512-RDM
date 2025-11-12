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
#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "rdmdevicestore.h"
#include "rdmconst.h"
#include "rdm_e120.h"
#include "json/rdmdeviceparams.h"
#if defined(H3)
#include "h3_board.h"
#elif defined(GD32)
#include "gd32_board.h"
#endif
#include "hal_serialnumber.h"
#include "debug.h"

struct TRDMDeviceInfoData
{
    char* data;
    uint8_t length;
};

class RDMDevice
{
#if defined(CONFIG_RDM_DEVICE_ROOT_LABEL)
    static constexpr char kDeviceLabel[] = CONFIG_RDM_DEVICE_ROOT_LABEL;
#else
#if defined(H3)
    static constexpr char kDeviceLabel[] = H3_BOARD_NAME " RDM Device";
#elif defined(GD32)
    static constexpr char kDeviceLabel[] = GD32_BOARD_NAME " RDM Device";
#elif defined(RASPPI)
    static constexpr char kDeviceLabel[] = "Raspberry Pi RDM Device";
#elif defined(__linux__)
    static constexpr char kDeviceLabel[] = "Linux RDM Device";
#elif defined(__APPLE__)
    static constexpr char kDeviceLabel[] = "MacOS RDM Device";
#else
    static constexpr char kDeviceLabel[] = "RDM Device";
#endif
#endif
   public:
    RDMDevice()
    {
        DEBUG_ENTRY
        assert(s_this == nullptr);
        s_this = this;

        hal::SerialNumber(s_serial_number);

        s_uid[0] = RDMConst::MANUFACTURER_ID[0];
        s_uid[1] = RDMConst::MANUFACTURER_ID[1];
        s_uid[2] = s_serial_number[0];
        s_uid[3] = s_serial_number[1];
        s_uid[4] = s_serial_number[2];
        s_uid[5] = s_serial_number[3];

        s_factory_root_label_length = sizeof(kDeviceLabel) - 1;
        memcpy(s_factory_root_label, kDeviceLabel, s_factory_root_label_length);

        DEBUG_EXIT
    }

    void Init()
    {
        DEBUG_ENTRY

        assert(!s_is_init);

        RDMDevice::SetFactoryDefaults();

        s_is_init = true;

        json::RdmDeviceParams rdmdevice_params;
        rdmdevice_params.Load();
        rdmdevice_params.Set();

        DEBUG_EXIT
    }

    void Print()
    {
        puts("RDM Device configuration");
        const auto kLength = static_cast<int>(std::min(static_cast<size_t>(RDM_MANUFACTURER_LABEL_MAX_LENGTH), strlen(RDMConst::MANUFACTURER_NAME)));
        printf(" Manufacturer Name : %.*s\n", kLength, const_cast<char*>(&RDMConst::MANUFACTURER_NAME[0]));
        printf(" Manufacturer ID   : %.2X%.2X\n", s_uid[0], s_uid[1]);
        printf(" Serial Number     : %.2X%.2X%.2X%.2X\n", s_serial_number[3], s_serial_number[2], s_serial_number[1], s_serial_number[0]);
        printf(" Root label        : %.*s\n", s_root_label_length, s_root_label);
        printf(" Product Category  : %.2X%.2X\n", s_product_category >> 8, s_product_category & 0xFF);
        printf(" Product Detail    : %.2X%.2X\n", s_product_detail >> 8, s_product_detail & 0xFF);
    }

    void SetFactoryDefaults()
    {
        DEBUG_ENTRY

        const struct TRDMDeviceInfoData kInfoData = {.data = s_factory_root_label, .length = s_factory_root_label_length};
        SetLabel(&kInfoData);

        s_checksum = RDMDevice::CalculateChecksum();

        DEBUG_EXIT
    }

    bool GetFactoryDefaults() { return (s_checksum == RDMDevice::CalculateChecksum()); }

    const uint8_t* GetUID() const { return s_uid; }

    const uint8_t* GetSN() const { return s_serial_number; }

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

        memcpy(s_root_label, info_data->data, kLength);
        s_root_label_length = kLength;

        if (s_is_init)
        {
            rdmdevice_store::SaveLabel(s_root_label, s_root_label_length);
        }
    }

    void GetLabel(struct TRDMDeviceInfoData* info_data)
    {
        info_data->data = s_root_label;
        info_data->length = s_root_label_length;
    }

    void SetProductCategory(uint16_t product_category) { s_product_category = product_category; }
    uint16_t GetProductCategory() const { return s_product_category; }

    void SetProductDetail(uint16_t product_detail) { s_product_detail = product_detail; }
    uint16_t GetProductDetail() const { return s_product_detail; }

    static RDMDevice* Get() { return s_this; }

   private:
    uint16_t CalculateChecksum()
    {
        uint16_t checksum = s_root_label_length;

        for (uint32_t i = 0; i < s_root_label_length; i++)
        {
            checksum = static_cast<uint16_t>(checksum + s_root_label[i]);
        }

        return checksum;
    }

   private:
    inline static char s_factory_root_label[RDM_DEVICE_LABEL_MAX_LENGTH];
    inline static char s_root_label[RDM_DEVICE_LABEL_MAX_LENGTH];

    inline static uint16_t s_product_category{E120_PRODUCT_CATEGORY_OTHER};
    inline static uint16_t s_product_detail{E120_PRODUCT_DETAIL_OTHER};
    inline static uint16_t s_checksum{0};

    inline static uint8_t s_uid[RDM_UID_SIZE];
#define DEVICE_SN_LENGTH 4
    inline static uint8_t s_serial_number[DEVICE_SN_LENGTH];
    inline static uint8_t s_factory_root_label_length{0};
    inline static uint8_t s_root_label_length{0};

    inline static bool s_is_init{false};

    inline static RDMDevice* s_this;
};
