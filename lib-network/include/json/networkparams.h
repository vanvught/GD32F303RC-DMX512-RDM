#pragma once
/**
 * @file networkparams.h
 */
/* Copyright (C) 2025 by Arjan van Vught mailto:info@gd32-dmx.org */

#include <cstdint>

#include "configurationstore.h"
#include "json/json_key.h"
#include "json/networkparamsconst.h"
#include "json/json_params_base.h"

namespace json
{
class NetworkParams : public JsonParamsBase<NetworkParams>
{
   public:
    NetworkParams();

    NetworkParams(const NetworkParams&) = delete;
    NetworkParams& operator=(const NetworkParams&) = delete;

    NetworkParams(NetworkParams&&) = delete;
    NetworkParams& operator=(NetworkParams&&) = delete;

    void Load() { JsonParamsBase::Load(NetworkParamsConst::kFileName); }
    void Store(const char* buffer, uint32_t buffer_size);
    void Set();

    uint32_t GetIpAddress() const { return store_network_.local_ip; }
    uint32_t GetNetMask() const { return store_network_.netmask; }
    uint32_t GetDefaultGateway() const { return store_network_.gateway_ip; }
    uint32_t GetNtpServer() const { return store_network_.ntp_server_ip; }
    const char* GetHostName() const { return reinterpret_cast<const char*>(store_network_.host_name); }

    bool IsDhcpUsed() const { return !store_network_.use_static_ip; }

   protected:
    void Dump();

   private:
    static void SetUseStaticIp(const char* val, uint32_t len);
    static void SetIpAddress(const char* val, uint32_t len);
    static void SetNetMask(const char* val, uint32_t len);
    static void SetDefaultGateway(const char* val, uint32_t len);
    static void SetHostname(const char* val, uint32_t len);
    static void SetNtpServer(const char* val, uint32_t len);
    
   	static constexpr json::Key kNetworkKeys[] = {
	json::MakeKey(SetUseStaticIp, NetworkParamsConst::kUseStaticIp), 
	json::MakeKey(SetIpAddress, NetworkParamsConst::kIpAddress), 
	json::MakeKey(SetNetMask, NetworkParamsConst::kNetMask),
	json::MakeKey(SetDefaultGateway, NetworkParamsConst::kDefaultGateway),
	json::MakeKey(SetHostname, NetworkParamsConst::kHostname),
	json::MakeKey(SetNtpServer, NetworkParamsConst::kNtpServer)
    };

    inline static common::store::Network store_network_;

    friend class JsonParamsBase<NetworkParams>;
};
} // namespace json