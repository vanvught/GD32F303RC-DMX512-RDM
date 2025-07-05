/**
 * @file spilcd.h
 *
 */
/* Copyright (C) 2022-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef SPI_SPILCD_H_
#define SPI_SPILCD_H_

#include "spi/config.h"

#include "hal_spi.h"

#include "debug.h"

#if defined CONFIG_LCD_SPI_BITBANG
# define SPI_PREFIX(x)	FUNC_PREFIX(bitbang_##x);
#else
# define SPI_PREFIX(x)	FUNC_PREFIX(x);
#endif

class SpiLcd {
public:
	SpiLcd(uint32_t nCS = 0) : m_nCS(nCS) {
		DEBUG_ENTRY
		DEBUG_PRINTF("nCS=%u", nCS);

		SPI_PREFIX(SpiBegin());
		SPI_PREFIX(SpiChipSelect(SPI_CS_NONE));
		SPI_PREFIX(SpiSetSpeedHz(20000000));
		SPI_PREFIX(SpiSetDataMode(SPI_MODE0));

#if defined (SPI_LCD_RST_GPIO)
		FUNC_PREFIX(GpioFsel(SPI_LCD_RST_GPIO, GPIO_FSEL_OUTPUT));
#endif
		FUNC_PREFIX(GpioFsel(SPI_LCD_DC_GPIO, GPIO_FSEL_OUTPUT));
		FUNC_PREFIX(GpioFsel(SPI_LCD_BL_GPIO, GPIO_FSEL_OUTPUT));
#if defined(SPI_LCD_HAVE_CS_GPIO)
		FUNC_PREFIX(GpioFsel(m_nCS, GPIO_FSEL_OUTPUT));
#endif

		DEBUG_EXIT
	}

	void HardwareReset() {
#if defined (SPI_LCD_RST_GPIO)
		udelay(1000 * 200);
		FUNC_PREFIX(GpioClr(SPI_LCD_RST_GPIO));
		udelay(1000 * 200);
		FUNC_PREFIX(GpioSet(SPI_LCD_RST_GPIO));
		udelay(1000 * 200);
#endif
	}

	void SetCS() {
#if defined(SPI_LCD_HAVE_CS_GPIO)
		FUNC_PREFIX(GpioSet(m_nCS));
#endif
	}

	void ClearCS() {
#if defined(SPI_LCD_HAVE_CS_GPIO)
		FUNC_PREFIX(GpioClr(m_nCS));
#endif
	}

	void SetDC() {
		FUNC_PREFIX(GpioSet(SPI_LCD_DC_GPIO));
	}

	void ClearDC() {
		FUNC_PREFIX(GpioClr(SPI_LCD_DC_GPIO));
	}

	void WriteCommand(uint8_t nData) {
		ClearCS();
		ClearDC();
		SPI_PREFIX(SpiWritenb(reinterpret_cast<char *>(&nData), 1));
		SetCS();
	}

	void WriteData(const uint8_t *pData, const uint32_t nLength) {
		ClearCS();
		SetDC();
		SPI_PREFIX(SpiWritenb(reinterpret_cast<const char *>(pData), nLength));
		SetCS();
	}

	void WriteCommand(const uint8_t *pData, const uint32_t nLength) {
		auto *p = pData;
		WriteCommand(p++[0]);
		if (nLength != 0)
			WriteData(p, nLength);
	}

	void WriteDataByte(uint8_t nData) {
		ClearCS();
		SetDC();
		SPI_PREFIX(SpiWritenb(reinterpret_cast<char *>(&nData), 1));
		SetCS();
	}

	void WriteDataWord(uint16_t nData) {
		ClearCS();
		SetDC();
		SPI_PREFIX(SpiWrite(nData));
		SetCS();
	}

	void WriteDataStart(uint8_t *pData, const uint32_t nLength) {
		ClearCS();
		SetDC();
		SPI_PREFIX(SpiWritenb(reinterpret_cast<char *>(pData), nLength));
	}

	void WriteDataContinue(uint8_t *pData, const uint32_t nLength) {
		SPI_PREFIX(SpiWritenb(reinterpret_cast<char *>(pData), nLength));
	}

	void WriteDataEnd(uint8_t *pData, const uint32_t nLength) {
		SPI_PREFIX(SpiWritenb(reinterpret_cast<char *>(pData), nLength));
		SetCS();
	}

private:
	uint32_t m_nCS;
};

#endif /* SPI_SPILCD_H_ */
