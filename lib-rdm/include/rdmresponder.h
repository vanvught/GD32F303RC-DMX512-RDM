#pragma once
/**
 * @file rdmresponder.h
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

#include <cstdint>
#include <cassert>

#include "rdm.h"
#include "rdmhandler.h"
#include "rdmdeviceresponder.h"
#include "rdmpersonality.h"

#include "dmxreceiver.h"

#include "dmxnode_outputtype.h"

#include "debug.h"

#if defined(NODE_RDMNET_LLRP_ONLY)
#error "Cannot be both RDMNet Device and RDM Responder"
#endif

namespace rdm::responder
{
static constexpr int NO_DATA = 0;
static constexpr int DISCOVERY_RESPONSE = -1;
static constexpr int INVALID_DATA_RECEIVED = -2;
static constexpr int INVALID_RESPONSE = -3;
} // namespace rdm::responder

namespace configstore
{
void delay();
} // namespace configstore

class RDMResponder final : DMXReceiver, public RDMDeviceResponder, RDMHandler
{
   public:
    RDMResponder(RDMPersonality** pRDMPersonalities, uint32_t nPersonalityCount,
                 const uint32_t nCurrentPersonality = rdm::device::responder::DEFAULT_CURRENT_PERSONALITY)
        : DMXReceiver(pRDMPersonalities[nCurrentPersonality - 1]->GetDmxNodeOutputType()),
          RDMDeviceResponder(pRDMPersonalities, nPersonalityCount, nCurrentPersonality)
    {
        assert(s_this == nullptr);
        s_this = this;
    }

    ~RDMResponder() override = default;

    void Init()
    {
        RDMDeviceResponder::Init();
        // There is no DMXReceiver::Init()
    }

    int Run()
    {
        int16_t length;

#if !defined(CONFIG_RDM_ENABLE_SUBDEVICES)
        DMXReceiver::Run(length);
#else
        const auto* pDmxDataIn = DMXReceiver::Run(length);

        if (RDMSubDevices::Get()->GetCount() != 0)
        {
            if (length == -1)
            {
                if (m_IsSubDeviceActive)
                {
                    RDMSubDevices::Get()->Stop();
                    m_IsSubDeviceActive = false;
                }
            }
            else if (pDmxDataIn != nullptr)
            {
                RDMSubDevices::Get()->SetData(pDmxDataIn, static_cast<uint16_t>(length));
                if (!m_IsSubDeviceActive)
                {
                    RDMSubDevices::Get()->Start();
                    m_IsSubDeviceActive = true;
                }
            }
        }
#endif

        const auto* rdm_data_in = Rdm::Receive(0);

        if (rdm_data_in == nullptr) [[likely]]
        {
            return rdm::responder::NO_DATA;
        }

#ifndef NDEBUG
        rdm::MessagePrint(pRdmDataIn);
#endif

        if (rdm_data_in[0] == E120_SC_RDM)
        {
            const auto* rdm_command = reinterpret_cast<const struct TRdmMessage*>(rdm_data_in);

            switch (rdm_command->command_class)
            {
                case E120_DISCOVERY_COMMAND:
                case E120_GET_COMMAND:
                case E120_SET_COMMAND:
                    HandleData(&rdm_data_in[1], reinterpret_cast<uint8_t*>(&s_RdmCommand));
                    return HandleResponse(reinterpret_cast<uint8_t*>(&s_RdmCommand));
                    break;
                default:
                    DEBUG_PUTS("RDM_RESPONDER_INVALID_DATA_RECEIVED");
                    return rdm::responder::INVALID_DATA_RECEIVED;
                    break;
            }
        }

        DEBUG_PUTS("RDM_RESPONDER_DISCOVERY_RESPONSE");
        return rdm::responder::DISCOVERY_RESPONSE;
    }

    void Print()
    {
        RDMDeviceResponder::Print();
        DMXReceiver::Print();
    }

    void Start()
    {
        // There is no RDMDeviceResponder::Start()
        DMXReceiver::Start();
    }

    void DmxDisableOutput(bool disable) { DMXReceiver::SetDisableOutput(disable); }

    static RDMResponder* Get() { return s_this; }

    void PersonalityUpdate(uint32_t nPersonality) __attribute__((weak));
    void DmxStartAddressUpdate(uint16_t nDmxStartAddress) __attribute__((weak));

   private:
    int HandleResponse(const uint8_t* response)
    {
        auto length = rdm::responder::INVALID_RESPONSE;

        if (response[0] == E120_SC_RDM)
        {
            const auto* p = reinterpret_cast<const struct TRdmMessage*>(response);
            length = static_cast<int>(p->message_length + RDM_MESSAGE_CHECKSUM_SIZE);
            Rdm::SendRawRespondMessage(0, response, static_cast<uint16_t>(length));
        }
        else if (response[0] == 0xFE)
        {
            length = sizeof(struct TRdmDiscoveryMsg);
            Rdm::SendDiscoveryRespondMessage(0, response, static_cast<uint16_t>(length));
        }

#ifndef NDEBUG
        if (length != INVALID_RESPONSE)
        {
            rdm::MessagePrint(response);
        }
#endif

        configstore::delay();
        return length;
    }

    void PersonalityUpdate(DmxNodeOutputType* dmx_node_output_type) override
    {
        DMXReceiver::SetDmxNodeOutputType(dmx_node_output_type);
        PersonalityUpdate(static_cast<uint32_t>(RDMDeviceResponder::GetPersonalityCurrent(RDM_ROOT_DEVICE)));
    }

    void DmxStartAddressUpdate() override { DmxStartAddressUpdate(RDMDeviceResponder::GetDmxStartAddress(RDM_ROOT_DEVICE)); }

   private:
    static inline RDMResponder* s_this;
    static inline TRdmMessage s_RdmCommand;
    static inline bool m_IsSubDeviceActive;
};
