/**
 * @file rdmdevice.cpp
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

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>

#include "rdmdevice.h"
#include "rdmconst.h"
#include "rdm_e120.h"
#if defined(NO_EMAC) || !defined(CONFIG_RDMDEVICE_UUID_IP)
#include "netif.h"
#endif
#include "debug.h"

#if defined(CONFIG_RDM_DEVICE_ROOT_LABEL)
static constexpr char kDeviceLabel[] = CONFIG_RDM_DEVICE_ROOT_LABEL;
#else
#if defined(H3)
#include "h3_board.h"
static constexpr char kDeviceLabel[] = H3_BOARD_NAME " RDM Device";
#elif defined(GD32)
#include "gd32_board.h"
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

RDMDevice::RDMDevice()
{
    DEBUG_ENTRY
    assert(s_this == nullptr);
    s_this = this;

    m_aUID[0] = RDMConst::MANUFACTURER_ID[0];
    m_aUID[1] = RDMConst::MANUFACTURER_ID[1];

#if defined(NO_EMAC) || !defined(CONFIG_RDMDEVICE_UUID_IP)
    uint8_t mac_address[6];
    netif::MacAddressCopyTo(mac_address);

    m_aUID[2] = mac_address[2];
    m_aUID[3] = mac_address[3];
    m_aUID[4] = mac_address[4];
    m_aUID[5] = mac_address[5];

    m_aSN[0] = m_aUID[5];
    m_aSN[1] = m_aUID[4];
    m_aSN[2] = m_aUID[3];
    m_aSN[3] = m_aUID[2];
#else
    // Calculated with GetUID and GetSN
#endif

    m_nFactoryRootLabelLength = sizeof(kDeviceLabel) - 1;
    memcpy(m_aFactoryRootLabel, kDeviceLabel, m_nFactoryRootLabelLength);

    DEBUG_EXIT
}

void RDMDevice::Print()
{
    puts("RDM Device configuration");
    const auto kLength = static_cast<int>(std::min(static_cast<size_t>(RDM_MANUFACTURER_LABEL_MAX_LENGTH), strlen(RDMConst::MANUFACTURER_NAME)));
    printf(" Manufacturer Name : %.*s\n", kLength, const_cast<char*>(&RDMConst::MANUFACTURER_NAME[0]));
    printf(" Manufacturer ID   : %.2X%.2X\n", m_aUID[0], m_aUID[1]);
    printf(" Serial Number     : %.2X%.2X%.2X%.2X\n", m_aSN[3], m_aSN[2], m_aSN[1], m_aSN[0]);
    printf(" Root label        : %.*s\n", m_nRootLabelLength, m_aRootLabel);
    printf(" Product Category  : %.2X%.2X\n", m_nProductCategory >> 8, m_nProductCategory & 0xFF);
    printf(" Product Detail    : %.2X%.2X\n", m_nProductDetail >> 8, m_nProductDetail & 0xFF);
}
