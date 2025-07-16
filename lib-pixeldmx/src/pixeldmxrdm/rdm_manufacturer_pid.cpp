/**
 * @file rdm_manufacturer_pid.cpp
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

#if defined (DEBUG_PIXELDMX)
# undef NDEBUG
#endif

#include <cstdint>
#include <cstring>

#include "rdm_manufacturer_pid.h"
#include "rdmhandler.h"
#include "rdm_e120.h"

#include "pixeltype.h"
#include "pixeldmxstore.h"

#include "debug.h"

#if !defined(OUTPUT_DMX_PIXEL)
# error
# endif

namespace rdm {
using E120_MANUFACTURER_PIXEL_TYPE = ManufacturerPid<0x8500>;
using E120_MANUFACTURER_PIXEL_COUNT = ManufacturerPid<0x8501>;
using E120_MANUFACTURER_PIXEL_GROUPING_COUNT = ManufacturerPid<0x8502>;
using E120_MANUFACTURER_PIXEL_MAP = ManufacturerPid<0x8503>;

struct PixelType {
    static constexpr char description[] = "Pixel type";
};

struct PixelCount {
    static constexpr char description[] = "Pixel count";
};

struct PixelGroupingCount {
    static constexpr char description[] = "Pixel grouping count";
};

struct PixelMap {
    static constexpr char description[] = "Pixel map";
};

constexpr char PixelType::description[];
constexpr char PixelCount::description[];
constexpr char PixelGroupingCount::description[];
constexpr char PixelMap::description[];
}  // namespace rdm

const rdm::ParameterDescription RDMHandler::PARAMETER_DESCRIPTIONS[] = {
		  { rdm::E120_MANUFACTURER_PIXEL_TYPE::kCode,
		    rdm::DEVICE_DESCRIPTION_MAX_LENGTH,
			E120_DS_ASCII,
#if defined (CONFIG_RDM_MANUFACTURER_PIDS_SET)
			E120_CC_GET_SET,
#else
			E120_CC_GET,
#endif
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			0,
			0,
			rdm::Description<rdm::PixelType, sizeof(rdm::PixelType::description)>::kValue,
			rdm::pdlParameterDescription(sizeof(rdm::PixelType::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_COUNT::kCode,
			2,
			E120_DS_UNSIGNED_WORD,
#if defined (CONFIG_RDM_MANUFACTURER_PIDS_SET)
			E120_CC_GET_SET,
#else
			E120_CC_GET,
#endif
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			__builtin_bswap32(pixel::defaults::COUNT),
			__builtin_bswap32(pixel::max::ledcount::RGB),
			rdm::Description<rdm::PixelCount, sizeof(rdm::PixelCount::description)>::kValue,
			rdm::pdlParameterDescription(sizeof(rdm::PixelCount::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_GROUPING_COUNT::kCode,
			2,
			E120_DS_UNSIGNED_WORD,
#if defined (CONFIG_RDM_MANUFACTURER_PIDS_SET)
			E120_CC_GET_SET,
#else
			E120_CC_GET,
#endif
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			__builtin_bswap32(pixel::defaults::COUNT),
			__builtin_bswap32(pixel::max::ledcount::RGB),
			rdm::Description<rdm::PixelGroupingCount, sizeof(rdm::PixelGroupingCount::description)>::kValue,
			rdm::pdlParameterDescription(sizeof(rdm::PixelGroupingCount::description))
		  },
		  { rdm::E120_MANUFACTURER_PIXEL_MAP::kCode,
			rdm::DEVICE_DESCRIPTION_MAX_LENGTH,
			E120_DS_ASCII,
#if defined (CONFIG_RDM_MANUFACTURER_PIDS_SET)
			E120_CC_GET_SET,
#else
			E120_CC_GET,
#endif
			0,
			E120_UNITS_NONE,
			E120_PREFIX_NONE,
			0,
			0,
			0,
			rdm::Description<rdm::PixelMap, sizeof(rdm::PixelMap::description)>::kValue,
			rdm::pdlParameterDescription(sizeof(rdm::PixelMap::description))
		  }
  };

uint32_t RDMHandler::GetParameterDescriptionCount() const {
	return sizeof(RDMHandler::PARAMETER_DESCRIPTIONS) / sizeof(RDMHandler::PARAMETER_DESCRIPTIONS[0]);
}

#include "pixeldmx.h"

namespace rdm {
bool HandleManufactureerPidGet(uint16_t pid, [[maybe_unused]] const ManufacturerParamData *in, ManufacturerParamData *out, uint16_t& reason) {
	DEBUG_PRINTF("pid=%x", __builtin_bswap16(pid));

	auto &pixel_configuration = PixelConfiguration::Get();
	auto &pixel_dmx_configuration = PixelDmxConfiguration::Get();

	switch (pid) {
	case rdm::E120_MANUFACTURER_PIXEL_TYPE::kCode: {
		const auto *string = ::pixel::GetType(pixel_configuration.GetType());
		out->nPdl = static_cast<uint8_t>(strlen(string));
		memcpy(out->pParamData, string, out->nPdl);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_COUNT::kCode: {
		const auto kCount = pixel_configuration.GetCount();
		out->nPdl = 2;
		out->pParamData[0] = static_cast<uint8_t>(kCount >> 8);
		out->pParamData[1] = static_cast<uint8_t>(kCount);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_GROUPING_COUNT::kCode: {
		const auto kGrouingCount = pixel_dmx_configuration.GetGroupingCount();
		out->nPdl = 2;
		out->pParamData[0] = static_cast<uint8_t>(kGrouingCount >> 8);
		out->pParamData[1] = static_cast<uint8_t>(kGrouingCount);
		return true;
	}
	case rdm::E120_MANUFACTURER_PIXEL_MAP::kCode: {
        const auto* string = ::pixel::GetMap(pixel_configuration.GetMap());
        out->nPdl = static_cast<uint8_t>(strlen(string));
		memcpy(out->pParamData, string, out->nPdl);
		return true;
	}
	default:
		break;
	}

	reason = E120_NR_UNKNOWN_PID;
	return false;
}
#if defined (CONFIG_RDM_MANUFACTURER_PIDS_SET)
// C++ attribute: maybe_unused (since C++17)
bool HandleManufactureerPidSet(const bool isBroadcast, uint16_t pid, const rdm::ParameterDescription &parameterDescription, const ManufacturerParamData *in, [[maybe_unused]] ManufacturerParamData *out, uint16_t& reason) {
	DEBUG_PRINTF("pid=%x", __builtin_bswap16(pid));

	if (isBroadcast) {
		return false;
	}

	switch (pid) {
	case rdm::E120_MANUFACTURER_PIXEL_COUNT::kCode: {
		if (in->nPdl == 2) {
			const uint16_t nCount = in->pParamData[1] | in->pParamData[0] << 8;

			if ((nCount < parameterDescription.min_value) && (nCount > parameterDescription.max_value)) {
				reason = E120_NR_DATA_OUT_OF_RANGE;
				return false;
			}

			dmxled_store::SaveCount(nCount);
			return true;
		}

		reason = E120_NR_FORMAT_ERROR;
		return false;
	}
	case rdm::E120_MANUFACTURER_PIXEL_GROUPING_COUNT::kCode: {
		if (in->nPdl == 2) {
			const uint16_t grouing_count = in->pParamData[1] | in->pParamData[0] << 8;

			if ((grouing_count < parameterDescription.min_value) && (grouing_count > parameterDescription.max_value)) {
				reason = E120_NR_DATA_OUT_OF_RANGE;
				return false;
			}

			dmxled_store::SaveGroupingCount(grouing_count);
			return true;
		}

		reason = E120_NR_FORMAT_ERROR;
		return false;
	}
	case rdm::E120_MANUFACTURER_PIXEL_MAP::kCode: {
		if (in->nPdl == 3) {
			const auto map = ::pixel::GetMap(reinterpret_cast<const char *>(in->pParamData));

			if (map == pixel::Map::UNDEFINED) {
				reason = E120_NR_DATA_OUT_OF_RANGE;
				return false;
			}

			dmxled_store::SaveMap(static_cast<uint8_t>(map));
			return true;
		}

		reason = E120_NR_FORMAT_ERROR;
		return false;
	}
	default:
		break;
	}

	reason = E120_NR_UNKNOWN_PID;
	return false;

}
#endif
}  // namespace rdm
