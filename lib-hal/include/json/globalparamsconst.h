#pragma once
/**
 * @file globalparamsconst.h
 */
/* Copyright (C) 2025 by Arjan van Vught mailto:info@gd32-dmx.org */

#include "json/json_key.h"

namespace json
{
struct GlobalParamsConst
{
    static constexpr char kFileName[] = "global.json";
    
	inline static constexpr json::SimpleKey kUtcOffset {
	    "utc_offset",
	    10,
	    Fnv1a32("utc_offset", 10)
	};
};
} // namespace json
