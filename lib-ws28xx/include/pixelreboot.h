/**
 * @file pixelreboot.h
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef PIXELREBOOT_H_
#define PIXELREBOOT_H_

#if defined (OUTPUT_DMX_PIXEL_MULTI) || defined (PIXELPATTERNS_MULTI)
# include "ws28xxmulti.h"
#else
# include "ws28xx.h"
#endif

#include "hardware.h"

class PixelReboot final : public RebootHandler {
public:
	PixelReboot() {}
	~PixelReboot() override {}

	void Run() override {
#if defined (OUTPUT_DMX_PIXEL_MULTI) || defined (PIXELPATTERNS_MULTI)
		WS28xxMulti::Get()->Blackout();
#else
		WS28xx::Get()->Blackout();
#endif
	}
};

#endif /* PIXELREBOOT_H_ */
