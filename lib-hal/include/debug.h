/**
 * @file debug.h
 *
 */
/* Copyright (C) 2018-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef DEBUG_H_
#define DEBUG_H_

#if !defined(NDEBUG)
#include <cstdio>

#define DEBUG_ENTRY()                                          \
    do                                                         \
    {                                                          \
        printf("-> %s:%s:%d\n", __FILE__, __func__, __LINE__); \
    } while (0)

#define DEBUG_EXIT()                                           \
    do                                                         \
    {                                                          \
        printf("<- %s:%s:%d\n", __FILE__, __func__, __LINE__); \
    } while (0)

#define DEBUG_PRINTF(fmt, ...)                                                                    \
    do                                                                                            \
    {                                                                                             \
        printf("%s() %s:%d: " fmt "\n", __func__, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

#define DEBUG_PUTS(msg)            \
    do                             \
    {                              \
        DEBUG_PRINTF("%s", (msg)); \
    } while (0)

#else

#define DEBUG_ENTRY() \
    do                \
    {                 \
    } while (0)
#define DEBUG_EXIT() \
    do               \
    {                \
    } while (0)
#define DEBUG_PRINTF(...) \
    do                    \
    {                     \
    } while (0)
#define DEBUG_PUTS(...) \
    do                  \
    {                   \
    } while (0)

#endif

#endif // DEBUG_H_
