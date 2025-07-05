#pragma once
/**
 * @file networkparams.h
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

#include "configurationstore.h"

class NetworkParams
{
   public:
    NetworkParams();

    NetworkParams(const NetworkParams&) = delete;
    NetworkParams& operator=(const NetworkParams&) = delete;

    NetworkParams(NetworkParams&&) = delete;
    NetworkParams& operator=(NetworkParams&&) = delete;

    void Load();
    void Load(const char* buffer, uint32_t length);

    void Builder(char* buffer, uint32_t length, uint32_t& size);

    uint32_t GetIpAddress() const { return store_network_.local_ip; }
    uint32_t GetNetMask() const { return store_network_.netmask; }
    uint32_t GetDefaultGateway() const { return store_network_.gateway_ip; }
    const char* GetHostName() const { return reinterpret_cast<const char*>(store_network_.host_name); }
    uint32_t GetNameServer() const { return store_network_.name_server_ip; }
    uint32_t GetNtpServer() const { return store_network_.ntp_server_ip; }

    bool IsDhcpUsed() const { return !(store_network_.use_static_ip); }

    static void StaticCallbackFunction(void* p, const char* s);

   private:
    void Dump();
    void CallbackFunction(const char* s);

   private:
    common::store::Network store_network_;
};
