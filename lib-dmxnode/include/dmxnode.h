#pragma once
/**
 * @file dmxnode.h
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
#include <cstring>
#include <cstdio>
#include <cassert>

#include "configurationstore.h"

namespace dmxnode
{
inline constexpr uint16_t kAddressInvalid = 0xFFFF;
inline constexpr uint32_t kStartAddressDefault = 1;
inline constexpr uint32_t kUniverseSize = 512;
inline constexpr uint8_t kDmxMaxValue = 255;
/*
 * Art-Net
 */
inline constexpr uint32_t kNodeNameLength = 64;
inline constexpr uint32_t kLabelNameLength = 18;
/*
 * sACN E1.31
 */
namespace priority
{
inline constexpr uint8_t kLowest = 1;
inline constexpr uint8_t kDefault = 100;
inline constexpr uint8_t kHighest = 200;
} // namespace priority

#if !defined(DMXNODE_PORTS)
inline constexpr uint32_t kMaxPorts = 1; // Default if not overridden
#else
inline constexpr uint32_t kMaxPorts = DMXNODE_PORTS; // From build config
#endif

#if !defined(CONFIG_DMXNODE_DMX_PORT_OFFSET)
inline constexpr uint32_t kDmxportOffset = 0; // Default if not overridden
#else
inline constexpr uint32_t kDmxportOffset = CONFIG_DMXNODE_DMX_PORT_OFFSET; // From build config
#endif

inline constexpr uint32_t kConfigPortCount = ((kMaxPorts - kDmxportOffset) <= common::store::dmxnode::kParamPorts) ? (kMaxPorts - kDmxportOffset) : common::store::dmxnode::kParamPorts;

enum class Personality
{
    NODE,
    ARTNET,
    SACN
};

enum class MergeMode
{
    HTP,
    LTP
};

enum class PortDirection
{
    kInput,
    kOutput,
    kDisable
};

enum class FailSafe
{
    HOLD,
    OFF,
    ON,
    PLAYBACK,
    RECORD
};

enum class OutputStyle
{
    kDelta,   ///< DMX frame is triggered
    kConstant ///< DMX output is continuous
};

enum class Rdm
{
    kDisable,
    kEnable
};

struct SlotInfo
{
    uint16_t category;
    uint8_t type;
};

inline Personality GetPersonality(const char* personality)
{
    assert(personality != nullptr);
    if (strncasecmp(personality, "node", 4) == 0)
    {
        return Personality::NODE;
    }

    if (strncasecmp(personality, "sacn", 4) == 0)
    {
        return Personality::SACN;
    }

    return Personality::ARTNET;
}

inline const char* GetPersonality(Personality personality)
{
    if (personality == Personality::NODE)
    {
        return "node";
    }

    if (personality == Personality::SACN)
    {
        return "sacn";
    }

    return "artnet";
}

inline MergeMode GetMergeMode(const char* merge_mode)
{
    assert(merge_mode != nullptr);
    if (strncasecmp(merge_mode, "ltp", 3) == 0)
    {
        return MergeMode::LTP;
    }

    return MergeMode::HTP;
}

inline const char* GetMergeMode(MergeMode merge_mode, bool to_upper = false)
{
    if (to_upper)
    {
        return (merge_mode == MergeMode::HTP) ? "HTP" : "LTP";
    }
    return (merge_mode == MergeMode::HTP) ? "htp" : "ltp";
}

inline const char* GetMergeMode(unsigned m, bool to_upper = false)
{
    return GetMergeMode(static_cast<MergeMode>(m), to_upper);
}

inline PortDirection GetPortDirection(const char* port_direction)
{
    assert(port_direction != nullptr);

    if (strncasecmp(port_direction, "input", 5) == 0)
    {
        return PortDirection::kInput;
    }

    if (strncasecmp(port_direction, "disable", 7) == 0)
    {
        return PortDirection::kDisable;
    }

    return PortDirection::kOutput;
}

inline const char* GetPortDirection(PortDirection port_direction)
{
    if (port_direction == PortDirection::kInput)
    {
        return "input";
    }

    if (port_direction == PortDirection::kDisable)
    {
        return "disable";
    }

    return "output";
}

inline FailSafe GetFailsafe(const char* failsafe)
{
    if (strncasecmp(failsafe, "hold", 4) == 0)
    {
        return FailSafe::HOLD;
    }

    if (strncasecmp(failsafe, "off", 3) == 0)
    {
        return FailSafe::OFF;
    }

    if (strncasecmp(failsafe, "on", 2) == 0)
    {
        return FailSafe::ON;
    }

    if (strncasecmp(failsafe, "playback", 8) == 0)
    {
        return FailSafe::PLAYBACK;
    }

    return FailSafe::HOLD;
}

inline const char* GetFailsafe(FailSafe failsafe)
{
    if (failsafe == FailSafe::OFF)
    {
        return "off";
    }
    else if (failsafe == FailSafe::ON)
    {
        return "on";
    }
    else if (failsafe == FailSafe::PLAYBACK)
    {
        return "playback";
    }

    return "hold";
}

inline OutputStyle GetOutputStyle(const char* output_style)
{
    assert(output_style != nullptr);
    if (strncasecmp(output_style, "const", 5) == 0)
    {
        return OutputStyle::kConstant;
    }

    return OutputStyle::kDelta;
}

inline const char* GetOutputStyle(OutputStyle output_style, bool to_upper = false)
{
    if (to_upper)
    {
        return (output_style == OutputStyle::kDelta) ? "DELTA" : "CONST";
    }
    return (output_style == OutputStyle::kDelta) ? "delta" : "const";
}

inline void GetShortNameDefault(uint32_t port_index, char* short_name)
{
    snprintf(short_name, kLabelNameLength - 1, "Port %u", static_cast<unsigned int>(1 + port_index));
}
} // namespace dmxnode
