#pragma once
/**
 * @file propertiesbuilder.h
 *
 */
/* Copyright (C) 2019-2025 by Arjan van Vught mailto:info@gd32-dmx.org
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

class PropertiesBuilder {
public:
	PropertiesBuilder(const char *pFileName, char *pBuffer, uint32_t nLength);

	bool Add(const char *property, bool bValue) {
		return Add(property, bValue, bValue);
	}

	template<typename T>
	bool Add(const char *property, const T x, bool is_set = true, int nPrecision = 1) {
		if (m_nSize >= m_nLength) {
			return false;
		}

		auto *p = &m_pBuffer[m_nSize];
		const auto nSize = static_cast<size_t>(m_nLength - m_nSize);

		auto i = add_part(p, nSize, property, x, is_set, nPrecision);

		if (i > static_cast<int>(nSize)) {
			return false;
		}

		m_nSize = static_cast<uint16_t>(m_nSize + i);

		return true;
	}

	template<typename T> int inline add_part(char *p, uint32_t nSize, const char *property, const T x, bool is_set, [[maybe_unused]] int nPrecision) {
		if (is_set || m_bJson) {
			if (m_bJson) {
				return snprintf(p, nSize, "\"%s\":%d,", property, static_cast<int>(x));
			} else {
				return snprintf(p, nSize, "%s=%d\n", property, static_cast<int>(x));
			}
		}

		return snprintf(p, nSize, "#%s=%d\n", property, static_cast<int>(x));
	}

	bool AddIpAddress(const char *property, uint32_t nValue, bool is_set = true);

	bool AddHex8(const char *property, uint8_t nValue, bool is_set = true) {
		return AddHex(property, nValue, is_set, 2);
	}

	bool AddHex16(const char *property, const uint16_t nValue16, bool is_set = true) {
		return AddHex(property, nValue16, is_set, 4);
	}

	bool AddHex16(const char *property, const uint8_t nValue[2], bool is_set = true) {
		const auto v = static_cast<uint16_t>((nValue[0] << 8) | nValue[1]);
		return AddHex16(property, v, is_set);
	}

	bool AddHex24(const char *property, const uint32_t nValue32, bool is_set = true) {
		return AddHex(property, nValue32, is_set, 6);
	}

	bool AddUtcOffset(const char *property, int32_t hours, uint32_t minutes);

	bool AddComment(const char *pComment);

	bool AddRaw(const char *pRaw);

	uint16_t GetSize() {
		if (m_bJson) {
			m_pBuffer[m_nSize - 1] = '}';
			m_pBuffer[m_nSize] = '}';
			m_nSize++;
		}
		return m_nSize;
	}

private:
	bool AddHex(const char *property, uint32_t nValue, const bool is_set, const int nWidth);

private:
	char *m_pBuffer;
	uint16_t m_nLength;
	uint16_t m_nSize { 0 };
	bool m_bJson;
};

template<> int inline PropertiesBuilder::add_part<float>(char *p, uint32_t nSize, const char *property, const float x, bool is_set, int nPrecision) {
	if (is_set || m_bJson) {
		if (m_bJson) {
			return snprintf(p, nSize, "\"%s\":%.*f,", property, nPrecision, x);
		} else {
			return snprintf(p, nSize, "%s=%.*f\n", property, nPrecision, x);
		}
	}

	return snprintf(p, nSize, "#%s=%.*f\n", property, nPrecision, x);
}

template<> int inline PropertiesBuilder::add_part<char*>(char *p, uint32_t nSize, const char *property, char* x, bool is_set, [[maybe_unused]] int nPrecision) {
	if (is_set || m_bJson) {
		if (m_bJson) {
			return snprintf(p, nSize, "\"%s\":\"%s\",", property, x);
		} else {
			return snprintf(p, nSize, "%s=%s\n", property, x);
		}
	}

	return snprintf(p, nSize, "#%s=%s\n", property, x);
}

template<> int inline PropertiesBuilder::add_part<const char*>(char *p, uint32_t nSize, const char *property, const char* x, bool is_set, int nPrecision) {
	return PropertiesBuilder::add_part(p, nSize, property, const_cast<char *>(x), is_set, nPrecision);
}
