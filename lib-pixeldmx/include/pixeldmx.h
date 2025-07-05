/**
 * @file pixeldmx.h
 */
/* Copyright (C) 2016-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef PIXELDMX_H_
#define PIXELDMX_H_

#if defined (DEBUG_PIXELDMX)
# if defined (NDEBUG)
#  undef NDEBUG
#  define _NDEBUG
# endif
#endif

#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC push_options
# pragma GCC optimize ("O3")
# pragma GCC optimize ("no-tree-loop-distribute-patterns")
# pragma GCC optimize ("-fprefetch-loop-arrays")
#endif

#include <cstdint>
#include <algorithm>
#include <cassert>

#include "pixeloutput.h"
#include "pixeldmxconfiguration.h"
#include "pixeldmxstore.h"

#if defined (PIXELDMXSTARTSTOP_GPIO)
# include "hal_gpio.h"
#endif

#include "dmxnode.h"

#include "debug.h"

#if defined (OUTPUT_DMX_PIXEL) && defined(RDM_RESPONDER) && !defined(NODE_ARTNET)
# include "dmxnodeoutputrdmpixel.h"
# define OVERRIDE override
class PixelDmx final : public DmxNodeOutputRdmPixel {
#else
# define OVERRIDE
# define SETDATA
class PixelDmx {
#endif
public:
	PixelDmx() {
		DEBUG_ENTRY

		assert(s_this == nullptr);
		s_this = this;

		PixelDmxConfiguration::Get().Validate(1);

		m_pOutputType = new OutputType();
		assert(m_pOutputType != nullptr);
		m_pOutputType->Blackout();

#if defined (PIXELDMXSTARTSTOP_GPIO)
		FUNC_PREFIX(GpioFsel(PIXELDMXSTARTSTOP_GPIO, GPIO_FSEL_OUTPUT));
		FUNC_PREFIX(GpioClr(PIXELDMXSTARTSTOP_GPIO));
#endif

		DEBUG_EXIT
	}

	~PixelDmx() OVERRIDE {
		DEBUG_ENTRY

		delete m_pOutputType;
		m_pOutputType = nullptr;

		DEBUG_EXIT
	}

	void Start([[maybe_unused]] uint32_t nPortIndex) OVERRIDE {
		if (started_) {
			return;
		}

		started_ = true;

#if defined (PIXELDMXSTARTSTOP_GPIO)
		FUNC_PREFIX(GpioSet(PIXELDMXSTARTSTOP_GPIO));
#endif
	}

	void Stop([[maybe_unused]] uint32_t nPortIndex) OVERRIDE {
		if (!started_) {
			return;
		}

		started_ = false;

#if defined (PIXELDMXSTARTSTOP_GPIO)
		FUNC_PREFIX(GpioClr(PIXELDMXSTARTSTOP_GPIO));
#endif
	}

#if defined(SETDATA)
	template<bool doUpdate>
	void SetData(uint32_t nPortIndex, const uint8_t *pData, uint32_t nLength)  {
		SetDataImpl<doUpdate>(nPortIndex, pData, nLength);
	}

	template<bool doUpdate>
	void SetDataImpl([[maybe_unused]] uint32_t nPortIndex, const uint8_t *pData, uint32_t nLength) {
#else
	void SetDataImpl([[maybe_unused]] uint32_t nPortIndex, const uint8_t *pData, uint32_t nLength, const bool doUpdate) OVERRIDE {

#endif
		assert(pData != nullptr);
		assert(nLength <= dmxnode::kUniverseSize);

		if (m_pOutputType->IsUpdating()) {
			return;
		}

		auto &pixelDmxConfiguration = PixelDmxConfiguration::Get();
		auto &portInfo = pixelDmxConfiguration.GetPortInfo();
		uint32_t d = 0;

#if !defined(DMXNODE_PORTS)
		static constexpr uint32_t nSwitch = 0;
#else
		const auto nSwitch = nPortIndex & 0x03;
#endif
		const auto nGroups = pixelDmxConfiguration.GetGroups();
#if !defined(DMXNODE_PORTS)
		static constexpr uint32_t beginIndex = 0;
#else
		const auto beginIndex = portInfo.nBeginIndexPort[nSwitch];
#endif
		const auto nChannelsPerPixel = pixelDmxConfiguration.GetLedsPerPixel();
		const auto endIndex = std::min(nGroups,
				(beginIndex + (nLength / nChannelsPerPixel)));

		if ((nSwitch == 0) && (nGroups < portInfo.nBeginIndexPort[1])) {
			d = (pixelDmxConfiguration.GetDmxStartAddress() - 1U);
		}

		const auto nGroupingCount = pixelDmxConfiguration.GetGroupingCount();

		if (nChannelsPerPixel == 3) {
			switch (pixelDmxConfiguration.GetMap()) {
			case pixel::Map::RGB:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength); j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 0], pData[d + 1], pData[d + 2]);
					}
					d = d + 3;
				}
				break;
			case pixel::Map::RBG:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength);
						j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 0], pData[d + 2], pData[d + 1]);
					}
					d = d + 3;
				}
				break;
			case pixel::Map::GRB:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength);
						j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 1], pData[d + 0], pData[d + 2]);
					}
					d = d + 3;
				}
				break;
			case pixel::Map::GBR:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength);
						j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 2], pData[d + 0], pData[d + 1]);
					}
					d = d + 3;
				}
				break;
			case pixel::Map::BRG:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength);
						j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 1], pData[d + 2], pData[d + 0]);
					}
					d = d + 3;
				}
				break;
			case pixel::Map::BGR:
				for (uint32_t j = beginIndex; (j < endIndex) && (d < nLength); j++) {
					auto const nPixelIndexStart = (j * nGroupingCount);
					for (uint32_t k = 0; k < nGroupingCount; k++) {
						m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d + 2], pData[d + 1], pData[d + 0]);
					}
					d = d + 3;
				}
				break;
			default:
				assert(0);
				__builtin_unreachable();
				break;
			}
		} else {
			assert(nChannelsPerPixel == 4);
			for (auto j = beginIndex; (j < endIndex) && (d < nLength); j++) {
				auto const nPixelIndexStart = (j * nGroupingCount);
				for (uint32_t k = 0; k < nGroupingCount; k++) {
					m_pOutputType->SetPixel(nPixelIndexStart + k, pData[d], pData[d + 1], pData[d + 2], pData[d + 3]);
				}
				d = d + 4;
			}
		}

#if !defined(DMXNODE_PORTS)
		if (doUpdate) {
			if (__builtin_expect((m_bBlackout), 0)) {
				return;
			}
			m_pOutputType->Update();
		}
#else
#if !defined (SETDATA)
#error
#endif
		if constexpr (doUpdate) {
			if (nPortIndex == portInfo.nProtocolPortIndexLast) {

				if (__builtin_expect((m_bBlackout), 0)) {
					return;
				}
				m_pOutputType->Update();
			}
		}
#endif
	}

	void Sync([[maybe_unused]] const uint32_t nPortIndex) {
	}

	void Sync() {
		assert(m_pOutputType != nullptr);
		m_pOutputType->Update();
	}

#if defined (OUTPUT_HAVE_STYLESWITCH)
			void SetOutputStyle([[maybe_unused]] const uint32_t nPortIndex, [[maybe_unused]] const dmxnode::OutputStyle outputStyle)  {}
			dmxnode::OutputStyle GetOutputStyle([[maybe_unused]] const uint32_t nPortIndex) const  {
				return dmxnode::OutputStyle::kDelta;
			}
#endif

	void Blackout(const bool bBlackout = true) {
		m_bBlackout = bBlackout;

		while (m_pOutputType->IsUpdating()) {
			// wait for completion
		}

		if (bBlackout) {
			m_pOutputType->Blackout();
		} else {
			m_pOutputType->Update();
		}
	}

	void FullOn() {
		while (m_pOutputType->IsUpdating()) {
			// wait for completion
		}

		m_pOutputType->FullOn();
	}

	void Print() OVERRIDE {
		PixelDmxConfiguration::Get().Print();
	}

	// RDM
	bool SetDmxStartAddress(uint16_t nDmxStartAddress) OVERRIDE {
		assert((nDmxStartAddress != 0) && (nDmxStartAddress <= dmxnode::kUniverseSize));

		auto &pixelDmxConfiguration = PixelDmxConfiguration::Get();

		if (nDmxStartAddress == pixelDmxConfiguration.GetDmxStartAddress()) {
			return true;
		}

		if ((nDmxStartAddress + pixelDmxConfiguration.GetDmxFootprint())
				> dmxnode::kUniverseSize) {
			return false;
		}

		if ((nDmxStartAddress != 0) && (nDmxStartAddress <= dmxnode::kUniverseSize)) {
			pixelDmxConfiguration.SetDmxStartAddress(nDmxStartAddress);
			dmxled_store::SaveDmxStartAddress(nDmxStartAddress);
			return true;
		}

		return false;
	}

	uint16_t GetDmxStartAddress() OVERRIDE {
		return PixelDmxConfiguration::Get().GetDmxStartAddress();
	}

	uint16_t GetDmxFootprint() OVERRIDE {
		return PixelDmxConfiguration::Get().GetDmxFootprint();
	}

	bool GetSlotInfo(uint16_t nSlotOffset, dmxnode::SlotInfo &slotInfo) OVERRIDE
	{
		auto &pixelDmxConfiguration = PixelDmxConfiguration::Get();

		if (nSlotOffset > pixelDmxConfiguration.GetDmxFootprint()) {
			return false;
		}

		slotInfo.type = 0x00;	// ST_PRIMARY

		switch (nSlotOffset % pixelDmxConfiguration.GetLedsPerPixel()) {
		case 0:
			slotInfo.category = 0x0205; // SD_COLOR_ADD_RED
			break;
		case 1:
			slotInfo.category = 0x0206; // SD_COLOR_ADD_GREEN
			break;
		case 2:
			slotInfo.category = 0x0207; // SD_COLOR_ADD_BLUE
			break;
		case 3:
			slotInfo.category = 0x0212; // SD_COLOR_ADD_WHITE
			break;
		default:
			__builtin_unreachable();
			break;
		}

		return true;
	}

	/*
	 * Art-Net ArtPollReply
	 */
	uint32_t GetUserData() {
		return 0;
	}
	uint32_t GetRefreshRate() {
		return 0;
	}

	static PixelDmx& Get() {
		assert(s_this != nullptr); // Ensure that s_this is valid
		return *s_this;
	}

private:
	OutputType *m_pOutputType { nullptr };

	bool started_ { false };
	bool m_bBlackout { false };

	static inline PixelDmx *s_this;
};

#undef OVERRIDE
#undef SETDATA
#if defined(__GNUC__) && !defined(__clang__)
# pragma GCC pop_options
#endif
#if defined (_NDEBUG)
# undef _NDEBUG
# define NDEBUG
#endif
#endif /* PIXELDMX_H_ */
