#if defined(JSON_NATIVE)
/**
 * @file jsongetenv.cpp
 *
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
#include <cstdio>
#include <cassert>

#include "configstore.h"
#include "globalparamsconst.h"
#include "utc.h"

#include "json.h"

void JsonGetEnv(void* buffer, uint32_t length, uint32_t& size)
{
    assert(buffer != nullptr);
    assert(length != 0);

    int32_t hours;
    uint32_t minutes;
    hal::utc::GetOffset(hours, minutes);

    char offset_string[12];
    snprintf(offset_string, sizeof(offset_string) - 1, "%c%.2d:%.2u", hours < 0 ? '-' : '+', hours, minutes);

    JsonDocument doc;
    doc[GlobalParamsConst::FILE_NAME][GlobalParamsConst::UTC_OFFSET] = offset_string;
    doc.shrinkToFit();

    size = serializeJson(doc, buffer, length);
}
#endif
