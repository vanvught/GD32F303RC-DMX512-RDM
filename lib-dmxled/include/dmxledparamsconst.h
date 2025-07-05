/**
 * @file dmxledparamsconst.h
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

#ifndef DMXLEDPARAMSCONST_H_
#define DMXLEDPARAMSCONST_H_

struct DmxLedParamsConst {
	static inline const char FILE_NAME[] = "dmxled.txt";

	static inline const char TYPE[] = "led_type";

	static inline const char MAP[] = "led_rgb_mapping";

	static inline const char LED_T0H[] = "led_t0h";
	static inline const char LED_T1H[] = "led_t1h";

	static inline const char COUNT[] = "led_count";
	static inline const char GROUPING_COUNT[] = "led_group_count";

	static inline const char SPI_SPEED_HZ[] = "clock_speed_hz";

	static inline const char GLOBAL_BRIGHTNESS[] = "global_brightness";

	static inline const char ACTIVE_OUT[] = "active_out";

	static inline const char TEST_PATTERN[] = "test_pattern";

	static inline const char GAMMA_CORRECTION[] = "gamma_correction";
	static inline const char GAMMA_VALUE[] = "gamma_value";

};

#endif /* DMXLEDPARAMSCONST_H_ */
