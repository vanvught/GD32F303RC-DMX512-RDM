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
void uart0_puts(const char *);

void ConsolePutc(int c) {
	uart0_putc(c);
}

void ConsolePuts(const char *s) {
	uart0_puts(s);
}

// https://github.com/shiena/ansicolor/blob/master/README.md

void ConsoleError(const char *s) {
	uart0_puts("\x1b[31m");
	uart0_puts(s);
	uart0_puts("\x1b[39m");
}

void ConsoleSetFgColour(uint32_t fg) {
	switch (fg) {
	case CONSOLE_BLACK:
		uart0_puts("\x1b[30m");
		break;
	case CONSOLE_RED:
		uart0_puts("\x1b[31m");
		break;
	case CONSOLE_GREEN:
		uart0_puts("\x1b[32m");
		break;
	case CONSOLE_YELLOW:
		uart0_puts("\x1b[33m");
		break;
	case CONSOLE_WHITE:
		uart0_puts("\x1b[37m");
		break;
	default:
		uart0_puts("\x1b[39m");
		break;
	}
}

void ConsoleSetBgColour(uint32_t bg) {
	switch (bg) {
	case CONSOLE_BLACK:
		uart0_puts("\x1b[40m");
		break;
	case CONSOLE_RED:
		uart0_puts("\x1b[41m");
		break;
	case CONSOLE_WHITE:
		uart0_puts("\x1b[47m");
		break;
	default:
		uart0_puts("\x1b[49m");
		break;
	}
}

void ConsoleWrite(const char *s, unsigned int n) {
	char c;

	while (((c = *s++) != 0) && (n-- != 0)) {
		ConsolePutc(static_cast<int>(c));
	}
}

void ConsoleStatus(uint32_t nColour, const char *s) {
	ConsoleSetFgColour(nColour);
	uart0_puts(s);
	ConsolePutc('\n');
	ConsoleSetFgColour(CONSOLE_DEFAULT);
}

void __attribute__((cold)) ConsoleInit() {
	uart0_init();

	ConsoleSetFgColour(CONSOLE_WHITE);
	ConsoleSetBgColour(CONSOLE_BLACK);
}
