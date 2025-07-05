/**
 * @file rdmsensorsparams.cpp
 *
 */
/* Copyright (C) 2020-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "rdmsensorsparams.h"
#include "rdmsensors.h"
#include "rdmsensorsconst.h"
#include "rdm_sensors.h"

#include "configstore.h"
#include "configurationstore.h"

#include "readconfigfile.h"
#include "sscan.h"
#include "propertiesbuilder.h"

#if defined(RDM_SENSORS_ENABLE)
#if !defined(CONFIG_RDM_SENSORS_DISABLE_BH170)
#include "rdmsensorbh1750.h"
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_MCP9808)
#include "rdmsensormcp9808.h"
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_HTU21D)
#include "rdmsensorhtu21dhumidity.h"
#include "rdmsensorhtu21dtemperature.h"
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_INA219)
#include "rdmsensorina219current.h"
#include "rdmsensorina219power.h"
#include "rdmsensorina219voltage.h"
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_SI7021)
#include "rdmsensorsi7021humidity.h"
#include "rdmsensorsi7021temperature.h"
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_THERMISTOR)
#include "rdmsensorthermistor.h"
#endif
#endif

#include "debug.h"

static void Update(const common::store::RdmSensors* store)
{
    ConfigStore::Instance().Store(store, &ConfigurationStore::rdm_sensors);
}

static void Copy(struct ::common::store::RdmSensors* store)
{
    ConfigStore::Instance().Copy(store, &ConfigurationStore::rdm_sensors);
}

RDMSensorsParams::RDMSensorsParams()
{
    DEBUG_ENTRY

    memset(&store_rdm_sensors_, 0, sizeof(struct common::store::RdmSensors));

    DEBUG_EXIT
}

void RDMSensorsParams::Load()
{
    DEBUG_ENTRY

#if !defined(DISABLE_FS)
    ReadConfigFile configfile(RDMSensorsParams::StaticCallbackFunction, this);

    if (configfile.Read(RDMSensorsConst::PARAMS_FILE_NAME))
    {
        Update(&store_rdm_sensors_);
    }
    else
#endif
    {
        Copy(&store_rdm_sensors_);
    }
    // Sanity check
    if (store_rdm_sensors_.devices >= common::store::rdm::sensors::kMaxDevices)
    {
        memset(&store_rdm_sensors_, 0, sizeof(struct common::store::RdmSensors));
    }

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMSensorsParams::Load(const char* buffer, uint32_t length)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);
    assert(length != 0);

    ReadConfigFile config(RDMSensorsParams::StaticCallbackFunction, this);

    config.Read(buffer, length);

    Update(&store_rdm_sensors_);

#ifndef NDEBUG
    Dump();
#endif
    DEBUG_EXIT
}

void RDMSensorsParams::Builder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

    assert(buffer != nullptr);

    Copy(&store_rdm_sensors_);

    PropertiesBuilder builder(RDMSensorsConst::PARAMS_FILE_NAME, buffer, length);

    for (uint32_t i = 0; i < static_cast<uint32_t>(rdm::sensors::Types::UNDEFINED); i++)
    {
        builder.AddHex8(rdm::sensors::GetTypeString(static_cast<rdm::sensors::Types>(i)), 0xFF, false);
    }

    for (uint32_t i = 0; i < store_rdm_sensors_.devices; i++)
    {
        const auto kType = static_cast<rdm::sensors::Types>(store_rdm_sensors_.entry[i].type);
        if (kType < rdm::sensors::Types::UNDEFINED)
        {
            builder.AddHex8(rdm::sensors::GetTypeString(kType), store_rdm_sensors_.entry[i].address, true);
        }
    }

    size = builder.GetSize();

    DEBUG_PRINTF("size=%d", size);
    DEBUG_EXIT
}

bool RDMSensorsParams::Add(RDMSensor* rdm_sensor)
{
    DEBUG_ENTRY

    if (rdm_sensor->Initialize())
    {
        RDMSensors::Get()->Add(rdm_sensor);
        DEBUG_EXIT
        return true;
    }

    delete rdm_sensor;

    DEBUG_EXIT
    return false;
}

void RDMSensorsParams::Set()
{
#if defined(RDM_SENSORS_ENABLE)
    DEBUG_ENTRY

    for (uint32_t i = 0; i < store_rdm_sensors_.devices; i++)
    {
        auto sensor_number = RDMSensors::Get()->GetCount();
        const auto kAddress = store_rdm_sensors_.entry[i].address;

        switch (static_cast<rdm::sensors::Types>(store_rdm_sensors_.entry[i].type))
        {
#if !defined(CONFIG_RDM_SENSORS_DISABLE_BH170)
            case rdm::sensors::Types::BH170:
                Add(new RDMSensorBH170(sensor_number, kAddress));
                break;
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_HTU21D)
            case rdm::sensors::Types::HTU21D:
                if (!Add(new RDMSensorHTU21DHumidity(sensor_number++, kAddress)))
                {
                    continue;
                }
                Add(new RDMSensorHTU21DTemperature(sensor_number, kAddress));
                break;
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_INA219)
            case rdm::sensors::Types::INA219:
                if (!Add(new RDMSensorINA219Current(sensor_number++, kAddress)))
                {
                    continue;
                }
                if (!Add(new RDMSensorINA219Power(sensor_number++, kAddress)))
                {
                    continue;
                }
                Add(new RDMSensorINA219Voltage(sensor_number, kAddress));
                break;
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_MCP9808)
            case rdm::sensors::Types::MCP9808:
                Add(new RDMSensorMCP9808(sensor_number, kAddress));
                break;
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_SI7021)
            case rdm::sensors::Types::SI7021:
                if (!Add(new RDMSensorSI7021Humidity(sensor_number++, kAddress)))
                {
                    continue;
                }
                Add(new RDMSensorSI7021Temperature(sensor_number, kAddress));
                break;
#endif
#if !defined(CONFIG_RDM_SENSORS_DISABLE_THERMISTOR)
            case rdm::sensors::Types::MCP3424:
                if (!Add(new RDMSensorThermistor(sensor_number, kAddress, 0, store_rdm_sensors_.calibrate[sensor_number])))
                {
                    continue;
                }
                sensor_number++;
                if (!Add(new RDMSensorThermistor(sensor_number, kAddress, 1, store_rdm_sensors_.calibrate[sensor_number])))
                {
                    continue;
                }
                sensor_number++;
                if (!Add(new RDMSensorThermistor(sensor_number, kAddress, 2, store_rdm_sensors_.calibrate[sensor_number])))
                {
                    continue;
                }
                sensor_number++;
                Add(new RDMSensorThermistor(sensor_number, kAddress, 3, store_rdm_sensors_.calibrate[sensor_number]));
                break;
#endif
            default:
                break;
        }
    }

    DEBUG_EXIT
#endif
}

void RDMSensorsParams::CallbackFunction(const char* line)
{
    assert(line != nullptr);

    if (store_rdm_sensors_.devices == common::store::rdm::sensors::kMaxSensors)
    {
        return;
    }

    char sensor_name[32];
    memset(sensor_name, 0, sizeof(sensor_name));

    uint8_t length = sizeof(sensor_name) - 1;
    uint8_t i2c_address = 0;
    uint8_t reserved;

    const auto kReturnCode = Sscan::I2c(line, sensor_name, length, i2c_address, reserved);

    if ((kReturnCode == Sscan::OK) && (sensor_name[0] != 0) && (length != 0))
    {
        DEBUG_PRINTF("{%.*s}:%d, address=0x%.2x", length, aSensorName, length, nI2cAddress);

        rdm::sensors::Types sensor_type;

        if ((sensor_type = rdm::sensors::GetTypeString(sensor_name)) == rdm::sensors::Types::UNDEFINED)
        {
            return;
        }

        uint32_t i;

        for (i = 0; i < store_rdm_sensors_.devices; i++)
        {
            if ((i2c_address != 0) && (store_rdm_sensors_.entry[i].address == i2c_address))
            {
                store_rdm_sensors_.entry[i].type = static_cast<uint8_t>(sensor_type);
                return;
            }
            if ((i2c_address == 0) && (store_rdm_sensors_.entry[i].type == static_cast<uint8_t>(sensor_type)))
            {
                return;
            }
        }

        store_rdm_sensors_.devices++;
        store_rdm_sensors_.entry[i].type = static_cast<uint8_t>(sensor_type);
        store_rdm_sensors_.entry[i].address = i2c_address;
    }
}

void RDMSensorsParams::StaticCallbackFunction(void* p, const char* s)
{
    assert(p != nullptr);
    assert(s != nullptr);

    (static_cast<RDMSensorsParams*>(p))->CallbackFunction(s);
}

void RDMSensorsParams::Dump()
{
    printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, RDMSensorsConst::PARAMS_FILE_NAME);

    for (uint32_t i = 0; i < store_rdm_sensors_.devices; i++)
    {
        printf(" %s 0x%.2x\n", rdm::sensors::GetTypeString(static_cast<rdm::sensors::Types>(store_rdm_sensors_.entry[i].type)),
               store_rdm_sensors_.entry[i].address);
    }

    for (uint32_t i = 0; i < rdm::sensors::MAX; i++)
    {
        printf("%2u %u\n", static_cast<unsigned int>(i), static_cast<unsigned int>(store_rdm_sensors_.calibrate[i]));
    }
}
