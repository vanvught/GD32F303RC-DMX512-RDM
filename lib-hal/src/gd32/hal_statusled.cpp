/**
 * @file hal_statusled.cpp
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

#if defined (DEBUG_HAL)
# undef NDEBUG
#endif

#include <cstdint>

#include "hal.h"
#include "hal_statusled.h"

#include "softwaretimers.h"

#include "gd32.h"

#include "debug.h"

static TimerHandle_t s_nTimerId = TIMER_ID_NONE;

#if !defined(HAL_HAVE_PORT_BIT_TOGGLE)
static int32_t s_nToggleLed = 1;
#endif

static void ledblink([[maybe_unused]] TimerHandle_t nHandle) {
#if defined(HAL_HAVE_PORT_BIT_TOGGLE)
	GPIO_TG(LED_BLINK_GPIO_PORT) = LED_BLINK_PIN;
#else
	s_nToggleLed = -s_nToggleLed;

	if (s_nToggleLed > 0) {
# if defined (CONFIG_LEDBLINK_USE_PANELLED)
		hal::panel_led_on(hal::panelled::ACTIVITY);
# else
		GPIO_BOP(LED_BLINK_GPIO_PORT) = LED_BLINK_PIN;
# endif
	} else {
# if defined (CONFIG_LEDBLINK_USE_PANELLED)
		hal::panel_led_off(hal::panelled::ACTIVITY);
# else
		GPIO_BC(LED_BLINK_GPIO_PORT) = LED_BLINK_PIN;
# endif
	}
#endif
}

namespace hal {
void statusled_set_frequency(const uint32_t nFrequencyHz) {
	DEBUG_ENTRY
	DEBUG_PRINTF("m_nTimerId=%d, nFreqHz=%u", s_nTimerId, nFrequencyHz);

	if (s_nTimerId == TIMER_ID_NONE) {
		s_nTimerId = SoftwareTimerAdd((1000U / nFrequencyHz), ledblink);
		DEBUG_EXIT
		return;
	}

	switch (nFrequencyHz) {
	case 0:
		SoftwareTimerDelete(s_nTimerId);
#if defined (CONFIG_LEDBLINK_USE_PANELLED)
		hal::panel_led_off(hal::panelled::ACTIVITY);
#else
		GPIO_BC(LED_BLINK_GPIO_PORT) = LED_BLINK_PIN;
#endif
		break;
# if !defined (CONFIG_HAL_USE_MINIMUM)
	case 1:
		SoftwareTimerChange(s_nTimerId, (1000U / 1));
		break;
	case 3:
		SoftwareTimerChange(s_nTimerId, (1000U / 3));
		break;
	case 5:
		SoftwareTimerChange(s_nTimerId, (1000U / 5));
		break;
	case 8:
		SoftwareTimerChange(s_nTimerId, (1000U / 8));
		break;
# endif
	case 255:
		SoftwareTimerDelete(s_nTimerId);
#if defined (CONFIG_LEDBLINK_USE_PANELLED)
		hal::panel_led_on(hal::panelled::ACTIVITY);
#else
		GPIO_BOP(LED_BLINK_GPIO_PORT) = LED_BLINK_PIN;
#endif
		break;
	default:
		SoftwareTimerChange(s_nTimerId, (1000U / nFrequencyHz));
		break;
	}

	DEBUG_EXIT
}
}  // namespace hal
