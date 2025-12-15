/**
 * @file rdmdeviceresponder.h
 *
 */
/* Copyright (C) 2018-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef RDMDEVICERESPONDER_H_
#define RDMDEVICERESPONDER_H_

#include <cstdint>
#include <cstring>

 #include "firmware/debug/debug_debug.h"
#include "hal.h"
#include "rdmconst.h"
#include "rdmdevice.h"
#include "rdmidentify.h"
#include "rdmpersonality.h"
#include "rdmsensors.h"
#include "rdmsubdevices.h"
#include "dmxnode.h"
#include "dmxnode_outputtype.h"
#include "firmwareversion.h"

namespace rdm::device::responder
{
static constexpr uint8_t kDefaultCurrentPersonality = 1;

///< http://rdm.openlighting.org/pid/display?manufacturer=0&pid=96
struct DeviceInfo
{
    uint8_t
        protocol_major; ///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a Sub-Device.
    uint8_t
        protocol_minor; ///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a Sub-Device.
    uint8_t device_model[2]; ///< This field identifies the Device Model ID of the Root Device or the Sub-Device. The Manufacturer shall not use the same ID to
                             ///< represent more than one unique model type.
    uint8_t product_category[2]; ///< Devices shall report a Product Category based on the products primary function.
    uint8_t software_version[4]; ///< This field indicates the Software Version ID for the device. The Software Version ID is a 32-bit value determined by the
                                 ///< Manufacturer.
    uint8_t
        dmx_footprint[2]; ///< If the DEVICE_INFO message is directed to a Sub-Device, then the response for this field contains the DMX512 Footprint for that
                          ///< Sub-Device. If the message is sent to the Root Device, it is the Footprint for the Root Device itself. If the Device or
                          ///< Sub-Device does not utilize Null Start Code packets for any control or functionality then it shall report a Footprint of 0x0000.
    uint8_t current_personality;  ///<
    uint8_t personality_count;    ///<
    uint8_t dmx_start_address[2]; ///< If the Device or Sub-Device that this message is directed to has a DMX512 Footprint of 0, then this field shall be set to
                                  ///< 0xFFFF.
    uint8_t sub_device_count[2];  ///< The response for this field shall always be same regardless of whether this message is directed to the Root Device or a
                                  ///< Sub-Device.
    uint8_t sensor_count;         ///< This field indicates the number of available sensors in a Root Device or Sub-Device. When this parameter is directed to a
                                  ///< Sub-Device, the reply shall be identical for any Sub-Device owned by a specific Root Device.
};
} // namespace rdm::device::responder
namespace configstore
{
void SetFactoryDefaults();
} // namespace configstore

class RDMDeviceResponder : public RDMDevice
{
    static constexpr char kLanguage[2] = {'e', 'n'};

   public:
    RDMDeviceResponder(RDMPersonality** personalities, uint32_t personality_count,
                       uint32_t current_personality = rdm::device::responder::kDefaultCurrentPersonality)
        : rdm_personalities_(personalities)
    {
        DEBUG_ENTRY();

        assert(s_this == nullptr);
        s_this = this;

        memset(&device_info_, 0, sizeof(struct rdm::device::responder::DeviceInfo));
        memset(&sub_device_info_, 0, sizeof(struct rdm::device::responder::DeviceInfo));

        device_info_.personality_count = static_cast<uint8_t>(personality_count);
        device_info_.current_personality = static_cast<uint8_t>(current_personality);

        assert(current_personality != 0);

        const auto* dmx_node_output_type = rdm_personalities_[current_personality - 1]->GetDmxNodeOutputType();

        if (dmx_node_output_type == nullptr)
        {
            dmx_start_address_factory_default_ = dmxnode::kAddressInvalid;
        }

        DEBUG_EXIT();
    }

    virtual ~RDMDeviceResponder() = default;

    void Init()
    {
        DEBUG_ENTRY();

        RDMDevice::Init();

        const auto kSoftwareVersionId = FirmwareVersion::Get()->GetVersionId();
        const auto kDeviceModel = hal::kBoardId;
        const auto kProductCategory = RDMDevice::GetProductCategory();
        const auto kSubDevices = rdm_sub_devices_.GetCount();

        device_info_.protocol_major = (E120_PROTOCOL_VERSION >> 8);
        device_info_.protocol_minor = static_cast<uint8_t>(E120_PROTOCOL_VERSION);
        device_info_.device_model[0] = static_cast<uint8_t>(kDeviceModel >> 8);
        device_info_.device_model[1] = static_cast<uint8_t>(kDeviceModel);
        device_info_.product_category[0] = static_cast<uint8_t>(kProductCategory >> 8);
        device_info_.product_category[1] = static_cast<uint8_t>(kProductCategory);
        device_info_.software_version[0] = static_cast<uint8_t>(kSoftwareVersionId >> 24);
        device_info_.software_version[1] = static_cast<uint8_t>(kSoftwareVersionId >> 16);
        device_info_.software_version[2] = static_cast<uint8_t>(kSoftwareVersionId >> 8);
        device_info_.software_version[3] = static_cast<uint8_t>(kSoftwareVersionId);

        assert(device_info_.current_personality != 0);
        auto* dmx_node_output_type = rdm_personalities_[device_info_.current_personality - 1]->GetDmxNodeOutputType();

        if (dmx_node_output_type == nullptr)
        {
            device_info_.dmx_footprint[0] = 0;
            device_info_.dmx_footprint[1] = 0;
            device_info_.dmx_start_address[0] = static_cast<uint8_t>(dmx_start_address_factory_default_ >> 8);
            device_info_.dmx_start_address[1] = static_cast<uint8_t>(dmx_start_address_factory_default_);
        }
        else
        {
            device_info_.dmx_footprint[0] = static_cast<uint8_t>(dmx_node_output_type->GetDmxFootprint() >> 8);
            device_info_.dmx_footprint[1] = static_cast<uint8_t>(dmx_node_output_type->GetDmxFootprint());
            device_info_.dmx_start_address[0] = static_cast<uint8_t>(dmx_node_output_type->GetDmxStartAddress() >> 8);
            device_info_.dmx_start_address[1] = static_cast<uint8_t>(dmx_node_output_type->GetDmxStartAddress());
        }

        device_info_.sub_device_count[0] = static_cast<uint8_t>(kSubDevices >> 8);
        device_info_.sub_device_count[1] = static_cast<uint8_t>(kSubDevices);
        device_info_.sensor_count = rdm_sensors_.GetCount();

        memcpy(&sub_device_info_, &device_info_, sizeof(struct rdm::device::responder::DeviceInfo));

        checksum_ = CalculateChecksum();

        DEBUG_EXIT();
    }

    void Print()
    {
        RDMDevice::Print();

        assert(device_info_.current_personality != 0);
        const auto* personality = rdm_personalities_[device_info_.current_personality - 1];
        assert(personality != nullptr);
        const char* personality_description = personality->GetDescription();
        const auto kPersonalityDescriptionLength = personality->GetDescriptionLength();

        puts("RDM Responder configuration");
        printf(" Protocol Version %d.%d\n", device_info_.protocol_major, device_info_.protocol_minor);
        printf(" DMX Address      : %d\n", (device_info_.dmx_start_address[0] << 8) + device_info_.dmx_start_address[1]);
        printf(" DMX Footprint    : %d\n", (device_info_.dmx_footprint[0] << 8) + device_info_.dmx_footprint[1]);
        printf(" Personality %d of %d [%.*s]\n", device_info_.current_personality, device_info_.personality_count, kPersonalityDescriptionLength,
               personality_description);
        printf(" Sub Devices      : %d\n", (device_info_.sub_device_count[0] << 8) + device_info_.sub_device_count[1]);
        printf(" Sensors          : %d\n", device_info_.sensor_count);
    }

    // E120_DEVICE_INFO				0x0060
    struct rdm::device::responder::DeviceInfo* GetDeviceInfo(uint16_t sub_device = RDM_ROOT_DEVICE)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            const auto* sub_device_info = rdm_sub_devices_.GetInfo(sub_device);

            if (sub_device_info != nullptr)
            {
                sub_device_info_.dmx_footprint[0] = static_cast<uint8_t>(sub_device_info->dmx_footprint >> 8);
                sub_device_info_.dmx_footprint[1] = static_cast<uint8_t>(sub_device_info->dmx_footprint);
                sub_device_info_.current_personality = sub_device_info->current_personality;
                sub_device_info_.personality_count = sub_device_info->personality_count;
                sub_device_info_.dmx_start_address[0] = static_cast<uint8_t>(sub_device_info->dmx_start_address >> 8);
                sub_device_info_.dmx_start_address[1] = static_cast<uint8_t>(sub_device_info->dmx_start_address);
                sub_device_info_.sensor_count = sub_device_info->sensor_count;
            }

            return &sub_device_info_;
        }

        // TODO(a): FIXME Quick fix
        const auto kProductCategory = RDMDevice::GetProductCategory();
        device_info_.product_category[0] = static_cast<uint8_t>(kProductCategory >> 8);
        device_info_.product_category[1] = static_cast<uint8_t>(kProductCategory);

        return &device_info_;
    }

    // E120_DEVICE_LABEL			0x0082
    void SetLabel(uint16_t sub_device, const char* label, uint8_t label_length)
    {
        struct TRDMDeviceInfoData info;

        if (label_length > RDM_DEVICE_LABEL_MAX_LENGTH)
        {
            label_length = RDM_DEVICE_LABEL_MAX_LENGTH;
        }

        if (sub_device != RDM_ROOT_DEVICE)
        {
            rdm_sub_devices_.SetLabel(sub_device, label, label_length);
            return;
        }

        info.data = const_cast<char*>(label);
        info.length = label_length;

        RDMDevice::SetLabel(&info);
    }

    void GetLabel(uint16_t sub_device, struct TRDMDeviceInfoData* info)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            rdm_sub_devices_.GetLabel(sub_device, info);
            return;
        }

        RDMDevice::GetLabel(info);
    }

    // E120_FACTORY_DEFAULTS		0x0090
    void SetFactoryDefaults()
    {
		DEBUG_ENTRY();
		
        RDMDevice::SetFactoryDefaults();

        assert(rdm_personalities_ != nullptr);

        SetPersonalityCurrent(RDM_ROOT_DEVICE, rdm::device::responder::kDefaultCurrentPersonality);
        SetDmxStartAddress(RDM_ROOT_DEVICE, dmx_start_address_factory_default_);

        memcpy(&sub_device_info_, &device_info_, sizeof(struct rdm::device::responder::DeviceInfo));

        rdm_sub_devices_.SetFactoryDefaults();

        checksum_ = CalculateChecksum();
        is_factory_defaults_ = true;

        configstore::SetFactoryDefaults();
        
        DEBUG_EXIT();
    }

    bool GetFactoryDefaults()
    {
        if (is_factory_defaults_)
        {
            if (!RDMDevice::GetFactoryDefaults())
            {
                is_factory_defaults_ = false;
                return false;
            }

            if (checksum_ != CalculateChecksum())
            {
                is_factory_defaults_ = false;
                return false;
            }

            if (!rdm_sub_devices_.GetFactoryDefaults())
            {
                is_factory_defaults_ = false;
                return false;
            }
        }

        return is_factory_defaults_;
    }

    // E120_LANGUAGE				0x00B0
    void SetLanguage(const char language[2])
    {
        language_[0] = language[0];
        language_[1] = language[1];
    }
    const char* GetLanguage() const { return language_; }

    // E120_SOFTWARE_VERSION_LABEL	0x00C0
    const char* GetSoftwareVersion() const { return FirmwareVersion::Get()->GetSoftwareVersion(); }

    uint32_t GetSoftwareVersionLength() const { return firmwareversion::length::kSoftwareVersion; }

    // E120_DMX_START_ADDRESS		0x00F0
    void SetDmxStartAddress(uint16_t sub_device, uint16_t dmx_start_address)
    {
        DEBUG_ENTRY();

        if (dmx_start_address == 0 || dmx_start_address > dmxnode::kUniverseSize) return;

        if (sub_device != RDM_ROOT_DEVICE)
        {
            rdm_sub_devices_.SetDmxStartAddress(sub_device, dmx_start_address);
            return;
        }

        const auto* personality = rdm_personalities_[device_info_.current_personality - 1];
        assert(personality != nullptr);

        auto* dmx_node_output_type = personality->GetDmxNodeOutputType();

        if (dmx_node_output_type != nullptr)
        {
            if (dmx_node_output_type->SetDmxStartAddress(dmx_start_address))
            {
                device_info_.dmx_start_address[0] = static_cast<uint8_t>(dmx_start_address >> 8);
                device_info_.dmx_start_address[1] = static_cast<uint8_t>(dmx_start_address);
            }

            DmxStartAddressUpdate();
        }

        DEBUG_EXIT();
    }

    uint16_t GetDmxStartAddress(uint16_t sub_device = RDM_ROOT_DEVICE)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            return rdm_sub_devices_.GetDmxStartAddress(sub_device);
        }

        return static_cast<uint16_t>((device_info_.dmx_start_address[0] << 8) + device_info_.dmx_start_address[1]);
    }

    // E120_SLOT_INFO				0x0120
    bool GetSlotInfo(uint16_t sub_device, uint16_t slot_offset, dmxnode::SlotInfo& slot_info)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            return false; // TODO(a): GetSlotInfo SubDevice
        }

        const auto* personality = rdm_personalities_[device_info_.current_personality - 1];
        auto* dmx_node_output_type = personality->GetDmxNodeOutputType();

        return dmx_node_output_type->GetSlotInfo(slot_offset, slot_info);
    }

    uint16_t GetDmxFootPrint(uint16_t sub_device = RDM_ROOT_DEVICE)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            return rdm_sub_devices_.GetDmxFootPrint(sub_device);
        }

        return static_cast<uint16_t>((device_info_.dmx_footprint[0] << 8) + device_info_.dmx_footprint[1]);
    }

    // Personalities
    RDMPersonality* GetPersonality(uint16_t sub_device, uint8_t personality)
    {
        assert(personality >= 1);

        if (sub_device != RDM_ROOT_DEVICE)
        {
            return rdm_sub_devices_.GetPersonality(sub_device, personality);
        }

        assert(personality <= device_info_.personality_count);

        return rdm_personalities_[personality - 1];
    }

    uint8_t GetPersonalityCount(uint16_t sub_device = RDM_ROOT_DEVICE)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            return rdm_sub_devices_.GetPersonalityCount(sub_device);
        }

        return device_info_.personality_count;
    }

    void SetPersonalityCurrent(uint16_t sub_device, uint8_t personality)
    {
        assert(personality >= 1);

        if (sub_device != RDM_ROOT_DEVICE)
        {
            rdm_sub_devices_.SetPersonalityCurrent(sub_device, personality);
            return;
        }

        device_info_.current_personality = personality;

        assert(personality <= device_info_.personality_count);

        const auto* p_personality = rdm_personalities_[personality - 1];
        assert(p_personality != nullptr);

        auto* dmx_node_output_type = p_personality->GetDmxNodeOutputType();

        if (dmx_node_output_type != nullptr)
        {
            device_info_.dmx_footprint[0] = static_cast<uint8_t>(dmx_node_output_type->GetDmxFootprint() >> 8);
            device_info_.dmx_footprint[1] = static_cast<uint8_t>(dmx_node_output_type->GetDmxFootprint());
            device_info_.dmx_start_address[0] = static_cast<uint8_t>(dmx_node_output_type->GetDmxStartAddress() >> 8);
            device_info_.dmx_start_address[1] = static_cast<uint8_t>(dmx_node_output_type->GetDmxStartAddress());

            PersonalityUpdate(dmx_node_output_type);
        }
    }

    uint8_t GetPersonalityCurrent(uint16_t sub_device = RDM_ROOT_DEVICE)
    {
        if (sub_device != RDM_ROOT_DEVICE)
        {
            return rdm_sub_devices_.GetPersonalityCurrent(sub_device);
        }

        return device_info_.current_personality;
    }

    virtual void PersonalityUpdate([[maybe_unused]] DmxNodeOutputType* dmx_node_output_type) {}
    virtual void DmxStartAddressUpdate() {}

    static RDMDeviceResponder* Get() { return s_this; }

   private:
    uint16_t CalculateChecksum()
    {
        auto checksum = static_cast<uint16_t>((device_info_.dmx_start_address[0] >> 8) + device_info_.dmx_start_address[1]);
        checksum = static_cast<uint16_t>(checksum + device_info_.current_personality);
        return checksum;
    }

   private:
    RDMIdentify rdm_identify_;
    RDMSensors rdm_sensors_;
    RDMSubDevices rdm_sub_devices_;
    RDMPersonality** rdm_personalities_;
    rdm::device::responder::DeviceInfo device_info_;
    rdm::device::responder::DeviceInfo sub_device_info_;
    char language_[2]{kLanguage[0], kLanguage[1]};
    bool is_factory_defaults_{true};
    uint16_t checksum_{0};
    uint16_t dmx_start_address_factory_default_{dmxnode::kStartAddressDefault};

    static inline RDMDeviceResponder* s_this;
};

#endif  // RDMDEVICERESPONDER_H_
