#pragma once
/**
 * @file netif.h
 * @brief Low-level network interface configuration and status functions.
 */
/* Copyright (C) 2025 by Arjan van Vught mailto:info@gd32-dmx.org
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
#include "net/ip4_address.h"

namespace netif
{

// --- Interface Identity ---

inline void MacAddressCopyTo(uint8_t* mac_address)
{
    __builtin_memcpy(mac_address, net::netif_hwaddr(), NETIF_MAX_HWADDR_LEN);
}

const char* GetIfName();
inline uint32_t GetIfindex() {
	return 1;
}

void SetHostName(const char* hostname);
inline const char* GetHostName()
{
    return net::globals::netif_default.hostname;
}

void SetDomainName(const char* domain);
const char* GetDomainName();

// --- Name Servers ---

uint32_t GetNameServer(uint32_t index);
inline uint32_t GetNameServers()
{
    return net::NAMESERVERS_COUNT;
}

// --- DHCP ---

inline bool IsDhcpCapable()
{
    return true;
}
inline bool IsDhcpKnown()
{
    return true;
}

void EnableDhcp();

inline bool IsDhcpUsed()
{
    return net::netif_dhcp();
}

// --- Addressing Mode Summary ---

inline char GetAddressingMode()
{
    if (net::netif_autoip()) return 'Z';                     // Zeroconf
    if (IsDhcpKnown()) return net::netif_dhcp() ? 'D' : 'S'; // DHCP or Static
    return 'U';                                              // Unknown
}

// --- Zeroconf / AutoIP ---

inline bool IsZeroconfCapable()
{
    return true;
}

void SetZeroconf();

inline bool IsZeroconfUsed()
{
    return net::netif_autoip();
}

} // namespace netif
