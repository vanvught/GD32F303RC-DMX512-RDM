/**
 * @file rdmsubdevicesparams.cpp
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
#include <cstring>
#include <cstdio>
#include <cassert>

#include "rdmsubdevicesparams.h"
#include "rdmsubdevices.h"
#include "rdmsubdevicesconst.h"
#include "rdm _subdevices.h"

#include "configstore.h"
#include "configurationstore.h"

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

#if defined(CONFIG_RDM_ENABLE_SUBDEVICES)
#if defined(store_rdm_subdevices_USE_SPI)
#include "spi/rdmsubdevicebw7fets.h"
#include "spi/rdmsubdevicebwdimmer.h"
#include "spi/rdmsubdevicebwdio.h"
#include "spi/rdmsubdevicebwlcd.h"
#include "spi/rdmsubdevicebwrelay.h"
#include "spi/rdmsubdevicemcp23s08.h"
#include "spi/rdmsubdevicemcp23s17.h"
#include "spi/rdmsubdevicemcp4822.h"
#include "spi/rdmsubdevicemcp4902.h"
#endif
#if defined(store_rdm_subdevices_USE_I2C)
#endif
#endif

#include "debug.h"

static void Update(const common::store::RdmSubdevices* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::rdm_subdevices);
}

static void Copy(struct ::common::store::RdmSubdevices* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::rdm_subdevices);
}

RDMSubDevicesParams::RDMSubDevicesParams()
{
    DEBUG_ENTRY

    memset(&store_rdm_subdevices_, 0, sizeof(struct ::common::store::RdmSubdevices));

    DEBUG_EXIT
}

void RDMSubDevicesParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(RDMSubDevicesParams::StaticCallbackFunction, this);

    if (configfile.Read(RDMSubDevicesConst::PARAMS_FILE_NAME))
    {
        Update(&store_rdm_subdevices_);
    }
    else
#endif
    {
        Copy(&store_rdm_subdevices_);
    }
    // Sanity check
    if (store_rdm_subdevices_.count >= rdm::subdevices::MAX)
    {
        memset(&store_rdm_subdevices_, 0, sizeof(struct ::common::store::RdmSubdevices));
    }

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMSubDevicesParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    ReadConfigFile config(RDMSubDevicesParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_rdm_subdevices_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMSubDevicesParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);

    Copy(&store_rdm_subdevices_);

    PropertiesBuilder builder(RDMSubDevicesConst::PARAMS_FILE_NAME, buffer, length);

    for (uint32_t count = 0; count < store_rdm_subdevices_.count; count++)
    {
        char buffer[32];
        const auto* p = &store_rdm_subdevices_.entry[count];
        snprintf(buffer, sizeof(buffer) - 1, "%u,%s,%u,%u,%u", p->chip_select, rdm::subdevices::GetTypeString(static_cast<rdm::subdevices::Types>(p->type)),
                 p->address, p->dmx_start_address, static_cast<unsigned int>(p->speed_hz));
        builder.AddRaw(buffer);
    }

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
    DEBUG_EXIT
}

bool RDMSubDevicesParams::Add(RDMSubDevice* rdm_sub_device)
{
    DEBUG_ENTRY

    if (rdm_sub_device->Initialize())
    {
        RDMSubDevices::Get()->Add(rdm_sub_device);
        DEBUG_EXIT
        return true;
    }

    delete rdm_sub_device;

    DEBUG_EXIT
    return false;
}

void RDMSubDevicesParams::Set()
{
#if defined(CONFIG_RDM_ENABLE_SUBDEVICES)
    for (uint32_t i = 0; i < store_rdm_subdevices_.count; i++)
    {
#if defined(store_rdm_subdevices_USE_SPI) || defined(store_rdm_subdevices_USE_I2C)
        const auto chip_select = store_rdm_subdevices_.entry[i].chip_select;
        const auto address = store_rdm_subdevices_.entry[i].address;
        const auto dmx_start_address = store_rdm_subdevices_.entry[i].dmx_start_address;
        const auto speed_hz = store_rdm_subdevices_.entry[i].speed_hz;
#endif
        switch (static_cast<rdm::subdevices::Types>(store_rdm_subdevices_.entry[i].type))
        {
#if defined(store_rdm_subdevices_USE_SPI)
            case rdm::subdevices::Types::BW7FETS:
                Add(new RDMSubDeviceBw7fets(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::BWDIMMER:
                Add(new RDMSubDeviceBwDimmer(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::BWDIO:
                Add(new RDMSubDeviceBwDio(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::BWLCD:
                Add(new RDMSubDeviceBwLcd(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::BWRELAY:
                Add(new RDMSubDeviceBwRelay(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::MCP23S08:
                Add(new RDMSubDeviceMCP23S08(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::MCP23S17:
                Add(new RDMSubDeviceMCP23S17(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::rdm::subdevices::Types::MCP4822:
                Add(new RDMSubDeviceMCP4822(dmx_start_address, chip_select, address, speed_hz));
                break;
            case rdm::subdevices::Types::MCP4902:
                Add(new RDMSubDeviceMCP4902(dmx_start_address, chip_select, address, speed_hz));
                break;
#endif
#if defined(store_rdm_subdevices_USE_I2C)
#endif
            default:
                break;
        }
    }
#endif
}

void RDMSubDevicesParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);

    DEBUG_PUTS(line);

    char sub_device_name[32];
    memset(sub_device_name, 0, sizeof(sub_device_name));

    char chip_select = 0;
    uint8_t length = sizeof(sub_device_name) - 1;
    uint8_t address = 0;
    uint16_t dmx_start_address;
    uint32_t speed_hz = 0;

    const auto kReturnCode = Sscan::Spi(line, chip_select, sub_device_name, length, address, dmx_start_address, speed_hz);

    DEBUG_PRINTF("nReturnCode=%u", static_cast<uint32_t>(nReturnCode));

    if ((kReturnCode == Sscan::OK) && (sub_device_name[0] != 0) && (length != 0))
    {
        DEBUG_PRINTF("{%.*s}:%d, chip_select=%d, address=%d, dmx_start_address=%d, speed_hz=%d", length, aSubDeviceName, static_cast<int>(length), chip_select,
                     address, dmx_start_address, speed_hz);

        rdm::subdevices::Types sub_device_type;

        if ((sub_device_type = rdm::subdevices::GetTypeString(sub_device_name)) == rdm::subdevices::Types::UNDEFINED)
        {
            return;
        }

        uint32_t i;

        for (i = 0; i < store_rdm_subdevices_.count; i++)
        {
            if ((store_rdm_subdevices_.entry[i].chip_select == static_cast<uint8_t>(chip_select)) &&
                (store_rdm_subdevices_.entry[i].type == static_cast<uint8_t>(sub_device_type)) && (store_rdm_subdevices_.entry[i].address == address))
            {
                return;
            }
        }

        if (store_rdm_subdevices_.count == common::store::rdm::subdevices::kMaxSubdevices)
        {
            return;
        }

        store_rdm_subdevices_.count++;
        store_rdm_subdevices_.entry[i].type = static_cast<uint8_t>(sub_device_type);
        store_rdm_subdevices_.entry[i].chip_select = static_cast<uint8_t>(chip_select);
        store_rdm_subdevices_.entry[i].address = address;
        store_rdm_subdevices_.entry[i].dmx_start_address = dmx_start_address;
        store_rdm_subdevices_.entry[i].speed_hz = speed_hz;
    }
}

void RDMSubDevicesParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<RDMSubDevicesParams*>(p))->CallbackFunction(s);
}

void RDMSubDevicesParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, RDMSubDevicesConst::PARAMS_FILE_NAME);

    for (uint32_t i = 0; i < store_rdm_subdevices_.count; i++)
    {
        printf(" %s 0x%.2x\n", rdm::subdevices::GetTypeString(static_cast<rdm::subdevices::Types>(store_rdm_subdevices_.entry[i].type)),
               store_rdm_subdevices_.entry[i].address);
    }
}
