#pragma once
/**
 * @file net.h
 * @brief Public API for network configuration and status. Wraps netif internals.
 */
/* Copyright (C) 2024-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "net/netif.h"
#include "net/udp.h"

namespace net
{

// --- Interface Configuration ---

void Set(ip4_addr_t ipaddr, ip4_addr_t netmask, ip4_addr_t gw, bool use_dhcp);

void SetPrimaryIp(uint32_t ip);
void SetSecondaryIp();
void SetNetmask(uint32_t netmask);
void SetGatewayIp(uint32_t gateway_ip);

// --- Accessors ---

inline uint32_t GetPrimaryIp()
{
    return net::netif_ipaddr();
}
inline uint32_t GetSecondaryIp()
{
    return net::netif_secondary_ipaddr();
}
inline uint32_t GetNetmask()
{
    return net::netif_netmask();
}
inline uint32_t GetGatewayIp()
{
    return net::netif_gw();
}
inline uint32_t GetBroadcastIp()
{
    return net::netif_broadcast_ipaddr();
}

inline uint32_t GetNetmaskCIDR()
{
    return static_cast<uint32_t>(__builtin_popcount(GetNetmask()));
}

inline bool IsValidIp(uint32_t ip)
{
    return (net::netif_ipaddr() & net::netif_netmask()) == (ip & net::netif_netmask());
}

// --- Zeroconf / AutoIP ---

void SetZeroconf();

// --- UDP / IP ---

namespace udp
{
inline int32_t Begin(uint16_t port, net::UdpCallbackFunctionPtr callback = nullptr)
{
    return net::udp_begin(port, callback);
}

inline int32_t End(uint16_t port)
{
    return net::udp_end(port);
}

inline void SendTo(int32_t handle, const void* buffer, uint32_t length, uint32_t to_ip, uint16_t remote_port)
{
    if (__builtin_expect((net::GetPrimaryIp() != 0), 1))
    { //TODO (a) FIXME
        net::udp_send(handle, reinterpret_cast<const uint8_t*>(buffer), length, to_ip, remote_port);
    }
}

inline void SendToTimestamp(int32_t handle, const void* buffer, uint32_t length, uint32_t to_ip, uint16_t remote_port)
{
    net::udp_send_timestamp(handle, reinterpret_cast<const uint8_t*>(buffer), length, to_ip, remote_port);
}

inline uint32_t RecvFrom(int32_t handle, const void** buffer, uint32_t* from_ip, uint16_t* from_port)
{
    return net::udp_recv2(handle, reinterpret_cast<const uint8_t**>(buffer), from_ip, from_port);
}
} // namespace udp

// --- Multicast / IGMP ---

namespace igmp
{
void JoinGroup(int32_t handle, uint32_t ip);
void LeaveGroup(int32_t handle, uint32_t ip);
} // namespace igmp

// --- Global Network Control ---

void Shutdown();

} // namespace net
