/**
 * @file console.cpp
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

#include <cstdint>

#include "console.h"

void uart0_init();
void uart0_putc(int);
void uart0_puts(const char*);

namespace console
{
void Putc(int c)
{
    uart0_putc(c);
}

void Puts(const char* s)
{
    uart0_puts(s);
}

// https://github.com/shiena/ansicolor/blob/master/README.md

void ConsoleError(const char* s)
{
    uart0_puts("\x1b[31m");
    uart0_puts(s);
    uart0_puts("\x1b[39m");
}

void ConsoleSetFgColour(Colours fg)
{
    switch (fg)
    {
        case console::Colours::kConsoleBlack:
            uart0_puts("\x1b[30m");
            break;
        case console::Colours::kConsoleRed:
            uart0_puts("\x1b[31m");
            break;
        case console::Colours::kConsoleGreen:
            uart0_puts("\x1b[32m");
            break;
        case console::Colours::kConsoleYellow:
            uart0_puts("\x1b[33m");
            break;
        case console::Colours::kConsoleWhite:
            uart0_puts("\x1b[37m");
            break;
        default:
            uart0_puts("\x1b[39m");
            break;
    }
}

void ConsoleSetBgColour(Colours bg)
{
    switch (bg)
    {
        case console::Colours::kConsoleBlack:
            uart0_puts("\x1b[40m");
            break;
        case console::Colours::kConsoleRed:
            uart0_puts("\x1b[41m");
            break;
        case console::Colours::kConsoleWhite:
            uart0_puts("\x1b[47m");
            break;
        default:
            uart0_puts("\x1b[49m");
            break;
    }
}

void ConsoleWrite(const char* s, unsigned int n)
{
    char c;

    while (((c = *s++) != 0) && (n-- != 0))
    {
        Putc(static_cast<int>(c));
    }
}

void ConsoleStatus(Colours colour, const char* s)
{
    ConsoleSetFgColour(colour);
    uart0_puts(s);
    Putc('\n');
    ConsoleSetFgColour(console::Colours::kConsoleDefault);
}

void __attribute__((cold)) Init()
{
    uart0_init();

    ConsoleSetFgColour(console::Colours::kConsoleWhite);
    ConsoleSetBgColour(console::Colours::kConsoleBlack);
}
} // namespace console
