/**
 * @file txt_handler.cpp
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

#if defined(DEBUG_PCA9685DMX)
#undef NDEBUG
#endif

#include <cstdint>

#include "dmxnode_outputtype.h"
#if defined(DMXNODE_OUTPUT_PIXEL)
#include "pixeldmxparams.h"
#endif
#if defined(OUTPUT_DMX_TLC59711)
#include "tlc59711dmxparams.h"
#endif
#include "debug.h"

void PixelDmxParamsBuilder(char* buffer, uint32_t length, uint32_t& size)
{
    DEBUG_ENTRY

#if defined(OUTPUT_DMX_TLC59711)
    bool bIsSetLedType = false;

    TLC59711DmxParams tlc5911params;
    tlc5911params.Load();
#if defined(DMXNODE_OUTPUT_PIXEL)
    if ((bIsSetLedType = tlc5911params.IsSetLedType()) == true)
    {
#endif
        tlc5911params.Builder(buffer, length, size);
#if defined(DMXNODE_OUTPUT_PIXEL)
    }
#endif

    if (!bIsSetLedType)
    {
#endif
#if defined(DMXNODE_OUTPUT_PIXEL)
        PixelDmxParams pixel_dmx_params;
        pixel_dmx_params.Builder(buffer, length, size);
#endif
#if defined(OUTPUT_DMX_TLC59711)
    }
#endif

    DEBUG_EXIT
}

void PixelDmxParamsLoad(char* buffer, uint32_t length)
{
    DEBUG_ENTRY

#if defined(OUTPUT_DMX_TLC59711)
    TLC59711DmxParams tlc59711params;
    tlc59711params.Load(buffer, length);

    DEBUG_PRINTF("tlc5911params.IsSetLedType()=%d", tlc59711params.IsSetLedType());

    if (!tlc59711params.IsSetLedType())
    {
#endif
#if defined(DMXNODE_OUTPUT_PIXEL)
        PixelDmxParams pixel_dmx_params;
        pixel_dmx_params.Load(buffer, length);
#endif
#if defined(OUTPUT_DMX_TLC59711)
    }
#endif

    DEBUG_EXIT
}
