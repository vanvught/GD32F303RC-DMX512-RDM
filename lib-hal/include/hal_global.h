#pragma once
/**
 * @file hal_global.h
 * @brief Defines the Global class for managing UTC offset configuration and validation.
 *
 * This file contains declarations and implementation details of the Global singleton class,
 * which is used to get and set the current UTC offset. It also contains validation logic
 * for standard and non-standard UTC offsets.
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

/**
 * @namespace global
 * @brief Contains global variables related to time configuration.
 */
namespace global
{
/**
 * @brief The current UTC offset in seconds.
 */
extern int32_t g_nUtcOffset;
} // namespace global

namespace hal
{

/**
 * @class Global
 * @brief Singleton class for managing and validating UTC offsets.
 *
 * The Global class provides methods to get and set the UTC offset in seconds.
 * It supports validation against both standard hourly offsets and known minute-level deviations.
 */
class Global
{
    /**
     * @struct UtcOffset
     * @brief Represents a valid non-standard UTC offset with hours and minutes.
     */
    struct UtcOffset
    {
        int32_t nHours;   ///< Hours part of the UTC offset.
        uint32_t nMinutes; ///< Minutes part of the UTC offset.
    };

    /**
     * @brief List of known valid UTC offsets that include minute-level precision.
     * @details Includes fractional-hour time zones such as +5:45 (Nepal) or +9:30 (Australia).
     * @see https://en.wikipedia.org/wiki/List_of_UTC_time_offsets
     */
    static constexpr UtcOffset kUtcValidOffets[] = {{-9, 30}, {-3, 30}, {3, 30}, {4, 30}, {5, 30}, {5, 45}, {6, 30}, {8, 45}, {9, 30}, {10, 30}, {12, 45}};

    static constexpr int32_t kUtcOffsetMin = -12; ///< Minimum valid UTC offset in hours.
    static constexpr int32_t kUtcOffsetMax = 14;  ///< Maximum valid UTC offset in hours.

   public:
    /**
     * @brief Get the singleton instance of the Global class.
     * @return Reference to the singleton Global object.
     */
    static Global& Instance()
    {
        static Global instance;
        return instance;
    }

    /**
     * @brief Gets the current UTC offset in seconds.
     * @return UTC offset in seconds.
     */
    int32_t GetUtcOffset() const { return ::global::g_nUtcOffset; }

    /**
     * @brief Retrieves the current UTC offset as hours and minutes.
     * @param[out] hours Output variable for the hour component.
     * @param[out] minutes Output variable for the minute component.
     */
    void GetUtcOffset(int32_t& hours, uint32_t& minutes) const
    {
        hours = ::global::g_nUtcOffset / 3600;

        if (hours > 0)
        {
            minutes = static_cast<uint32_t>((::global::g_nUtcOffset - (hours * 3600))) / 60U;
        }
        else
        {
            minutes = static_cast<uint32_t>(((hours * 3600) - ::global::g_nUtcOffset)) / 60U;
        }
    }

    bool GetUtcOffset(const char* buffer, uint32_t buffer_length, int32_t& hours, uint32_t& minutes)
    {
        if (buffer_length != 6 || buffer == nullptr)
        {
            return false;
        }

        // Format expected: ±HH:MM
        if ((buffer[0] != '+') && (buffer[0] != '-'))
        {
            return false;
        }

        const bool kIsNegative = (buffer[0] == '-');

        // Validate HH
        if (buffer[1] < '0' || buffer[1] > '1') return false;
        if (buffer[2] < '0' || buffer[2] > '9') return false;

        int32_t h = (buffer[1] - '0') * 10 + (buffer[2] - '0');
        if (h > 14) return false;

        if (buffer[3] != ':') return false;

        // Validate MM
        if (buffer[4] < '0' || buffer[4] > '5') return false;
        if (buffer[5] < '0' || buffer[5] > '9') return false;

        uint32_t m = static_cast<uint32_t>((buffer[4] - '0') * 10 + (buffer[5] - '0'));
        if (m >= 60) return false;

        hours = kIsNegative ? -h : h;
        minutes = m;

        return IsValidUtcOffset(hours, minutes);
    }

    /**
     * @brief Sets the UTC offset in seconds, validating against allowed offsets.
     * @param utc_offset UTC offset in seconds.
     * @return true if the offset is valid and was set; false otherwise.
     */
    bool SetUtcOffset(int32_t utc_offset)
    {
        if (IsValidUtcOffset(utc_offset))
        {
            ::global::g_nUtcOffset = utc_offset;
            return true;
        }
        return false;
    }

    /**
     * @brief Sets the UTC offset from hours and minutes, internally converting to seconds.
     * @param hours Signed hour component.
     * @param minutes Unsigned minute component.
     * @return true if the combined offset is valid and was set; false otherwise.
     */
    bool SetUtcOffset(int32_t hours, uint32_t minutes) { return SetUtcOffset(ToUtcOffsetInSeconds(hours, minutes)); }

   private:
    /**
     * @brief Constructor (private for singleton pattern).
     */
    Global() = default;

    // Delete copy/move constructors and assignment operators to enforce singleton behavior.
    Global(const Global&) = delete;
    Global& operator=(const Global&) = delete;
    Global(Global&&) = delete;
    Global& operator=(Global&&) = delete;

    static constexpr int32_t ToUtcOffsetInSeconds(int32_t hours, uint32_t minutes)
    {
        const int32_t kTotalMinutes = static_cast<int32_t>(minutes);
        return (hours >= 0) ? (hours * 3600 + kTotalMinutes * 60) : (hours * 3600 - kTotalMinutes * 60);
    }

    bool IsValidUtcOffset(int32_t hours, uint32_t minutes) { return IsValidUtcOffset(ToUtcOffsetInSeconds(hours, minutes)); }

    /**
     * @brief Validates if a given UTC offset is allowed based on known values.
     * @param nOffset UTC offset in seconds.
     * @return true if the offset is valid; false otherwise.
     */
    bool IsValidUtcOffset(int32_t utc_offset)
    {
        if (utc_offset == 0) return true;

        const int32_t kHours = utc_offset / 3600;
        const auto kMinutes = static_cast<uint32_t>(utc_offset - (kHours * 3600));

        // Whole-hour offset within [-12, 14]
        if ((kMinutes == 0) && (kHours >= kUtcOffsetMin && kHours <= kUtcOffsetMax))
        {
            return true;
        }

        // Check known valid partial offsets
        for (const auto& offset : kUtcValidOffets)
        {
            const auto kTotal = ToUtcOffsetInSeconds(offset.nHours, offset.nMinutes);
            if (utc_offset == kTotal)
            {
                return true;
            }
        }
        return false;
    }
};

} // namespace hal
