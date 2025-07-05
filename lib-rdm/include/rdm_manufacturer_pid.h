#pragma once
/**
 * @file rdm_manufacturer_pid.h
 *
 */
/* Copyright (C) 2023-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <cstddef>
#include <cstdint>

#if !defined(PACKED)
#define PACKED __attribute__((packed))
#endif

namespace rdm
{
struct ParameterDescription
{
    const uint16_t pid;
    const uint8_t pdl_size;
    const uint8_t data_type;
    const uint8_t command_class;
    const uint8_t type;
    const uint8_t unit;
    const uint8_t prefix;
    const uint32_t min_value;
    const uint32_t default_value;
    const uint32_t max_value;
    const char* description;
    const uint8_t pdl;
} PACKED;

static constexpr uint8_t pdlParameterDescription(const size_t N)
{
    return static_cast<uint8_t>(sizeof(ParameterDescription) - sizeof(const char*) - sizeof(const uint8_t) + N);
}

static constexpr uint32_t DEVICE_DESCRIPTION_MAX_LENGTH = 32;

template <uint16_t Value> struct ManufacturerPid
{
    static constexpr uint16_t kCode = __builtin_bswap16(Value);
    static_assert(kCode >= __builtin_bswap16(0x8000) && kCode <= __builtin_bswap16(0xFFDF), "The manufacturer specific PID must be in range 0x8000 to 0xFFDF");
};

template <typename T, size_t N> struct Description
{
    static constexpr auto kSize = N - 1U;
    static_assert(kSize <= DEVICE_DESCRIPTION_MAX_LENGTH, "Description is too long");
    static constexpr char const* kValue = T::description;
};

size_t GetTableSize();

struct ManufacturerParamData
{
    uint8_t nPdl;
    uint8_t* pParamData;
};

bool HandleManufactureerPidGet(uint16_t pid, const ManufacturerParamData* in, ManufacturerParamData* out, uint16_t& reason);
bool HandleManufactureerPidSet(bool is_broadcast, uint16_t pid, const rdm::ParameterDescription& parameter_description,
                                  const ManufacturerParamData* in, ManufacturerParamData* out, uint16_t& reason);
} // namespace rdm
