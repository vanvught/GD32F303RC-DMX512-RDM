/**
 * @file dmxnodeparamsconst.h
 */
/* Copyright (C) 2025 by Arjan van Vught mailto:info@gd32-dmx.org */

#ifndef JSON_DMXNODEPARAMSCONST_H_
#define JSON_DMXNODEPARAMSCONST_H_

#include "json/json_key.h"

namespace json
{
struct DmxNodeParamsConst
{
    static constexpr char kFileName[] = "dmxnode.json";

	inline static constexpr json::SimpleKey kPersonality {
	    "personality",
	    11,
	    Fnv1a32("personality", 11)
	};
	
	inline static constexpr json::SimpleKey kNodeName {
	    "node_name",
	    8,
	    Fnv1a32("node_name", 8)
	};
	
	inline static constexpr json::SimpleKey kFailsafe {
	    "failsafe",
	    8,
	    Fnv1a32("failsafe", 8)
	};
	
	inline static constexpr json::SimpleKey kDisableMergeTimeout {
	    "disable_merge_timeout",
	    21,
	    Fnv1a32("disable_merge_timeout", 21)
	};

    inline static constexpr json::PortKey kLabelPortA{"label_port_a", 12, Fnv1a32("label_port_a", 12)};
#if (DMXNODE_PORTS > 1)
    inline static constexpr json::PortKey kLabelPortB{"label_port_b", 12, Fnv1a32("label_port_b", 12)};
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
    inline static constexpr json::PortKey kLabelPortC{"label_port_c", 12, Fnv1a32("label_port_c", 12)};
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
    inline static constexpr json::PortKey kLabelPortD{"label_port_d", 12, Fnv1a32("label_port_d", 12)};
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
    inline static constexpr json::PortKey kLabelPortE{"label_port_e", 12, Fnv1a32("label_port_e", 12)};
    inline static constexpr json::PortKey kLabelPortF{"label_port_f", 12, Fnv1a32("label_port_f", 12)};
    inline static constexpr json::PortKey kLabelPortG{"label_port_g", 12, Fnv1a32("label_port_g", 12)};
    inline static constexpr json::PortKey kLabelPortH{"label_port_h", 12, Fnv1a32("label_port_h", 12)};
#endif // (DMXNODE_PORTS == 8)

    static constexpr json::PortKey kLabelPort[] = 
    {
        kLabelPortA,
#if (DMXNODE_PORTS > 1)
        kLabelPortB,
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
        kLabelPortC,
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
        kLabelPortD,
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
        kLabelPortE, 
        kLabelPortF, 
        kLabelPortG,
        kLabelPortH
#endif // (DMXNODE_PORTS == 8)
    };

    inline static constexpr json::PortKey kUniversePortA{"universe_port_a", 15, Fnv1a32("universe_port_a", 15)};
#if (DMXNODE_PORTS > 1)
    inline static constexpr json::PortKey kUniversePortB{"universe_port_b", 15, Fnv1a32("universe_port_b", 15)};
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
    inline static constexpr json::PortKey kUniversePortC{"universe_port_c", 15, Fnv1a32("universe_port_c", 15)};
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
    inline static constexpr json::PortKey kUniversePortD{"universe_port_d", 15, Fnv1a32("universe_port_d", 15)};
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
    inline static constexpr json::PortKey kUniversePortE{"universe_port_e", 15, Fnv1a32("universe_port_e", 15)};
    inline static constexpr json::PortKey kUniversePortF{"universe_port_f", 15, Fnv1a32("universe_port_f", 15)};
    inline static constexpr json::PortKey kUniversePortG{"universe_port_g", 15, Fnv1a32("universe_port_g", 15)};
    inline static constexpr json::PortKey kUniversePortH{"universe_port_h", 15, Fnv1a32("universe_port_h", 15)};
#endif // (DMXNODE_PORTS == 8)

    static constexpr json::PortKey kUniversePort[] = 
    {
        kUniversePortA,
#if (DMXNODE_PORTS > 1)
        kUniversePortB,
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
        kUniversePortC,
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
        kUniversePortD,
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
        kUniversePortE, 
        kUniversePortF, 
        kUniversePortG,
        kUniversePortH
#endif // (DMXNODE_PORTS == 8)
    };

    inline static constexpr json::PortKey kDirectionPortA{"direction_port_a", 16, Fnv1a32("direction_port_a", 16)};
#if (DMXNODE_PORTS > 1)
    inline static constexpr json::PortKey kDirectionPortB{"direction_port_b", 16, Fnv1a32("direction_port_b", 16)};
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
    inline static constexpr json::PortKey kDirectionPortC{"direction_port_c", 16, Fnv1a32("direction_port_c", 16)};
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
    inline static constexpr json::PortKey kDirectionPortD{"direction_port_d", 16, Fnv1a32("direction_port_d", 16)};
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
    inline static constexpr json::PortKey kDirectionPortE{"direction_port_e", 16, Fnv1a32("direction_port_e", 16)};
    inline static constexpr json::PortKey kDirectionPortF{"direction_port_f", 16, Fnv1a32("direction_port_f", 16)};
    inline static constexpr json::PortKey kDirectionPortG{"direction_port_g", 16, Fnv1a32("direction_port_g", 16)};
    inline static constexpr json::PortKey kDirectionPortH{"direction_port_h", 16, Fnv1a32("direction_port_h", 16)};
#endif // (DMXNODE_PORTS == 8)

    static constexpr json::PortKey kDirectionPort[] = 
    {
        kDirectionPortA,
#if (DMXNODE_PORTS > 1)
        kDirectionPortB,
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
        kDirectionPortC,
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
        kDirectionPortD,
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
        kDirectionPortE, 
        kDirectionPortF, 
        kDirectionPortG,
        kDirectionPortH
#endif // (DMXNODE_PORTS == 8)
    };

    inline static constexpr json::PortKey kMergeModePortA{"merge_mode_port_a", 17, Fnv1a32("merge_mode_port_a", 17)};
#if (DMXNODE_PORTS > 1)
    inline static constexpr json::PortKey kMergeModePortB{"merge_mode_port_b", 17, Fnv1a32("merge_mode_port_b", 17)};
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
    inline static constexpr json::PortKey kMergeModePortC{"merge_mode_port_c", 17, Fnv1a32("merge_mode_port_c", 17)};
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
    inline static constexpr json::PortKey kMergeModePortD{"merge_mode_port_d", 17, Fnv1a32("merge_mode_port_d", 17)};
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
    inline static constexpr json::PortKey kMergeModePortE{"merge_mode_port_e", 17, Fnv1a32("merge_mode_port_e", 17)};
    inline static constexpr json::PortKey kMergeModePortF{"merge_mode_port_f", 17, Fnv1a32("merge_mode_port_f", 17)};
    inline static constexpr json::PortKey kMergeModePortG{"merge_mode_port_g", 17, Fnv1a32("merge_mode_port_g", 17)};
    inline static constexpr json::PortKey kMergeModePortH{"merge_mode_port_h", 17, Fnv1a32("merge_mode_port_h", 17)};
#endif // (DMXNODE_PORTS == 8)

    static constexpr json::PortKey kMergeModePort[] = 
    {
		kMergeModePortA,
#if (DMXNODE_PORTS > 1)
        kMergeModePortB,
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
        kMergeModePortC,
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
        kMergeModePortD,
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
        kMergeModePortE, 
        kMergeModePortF, 
        kMergeModePortG,
        kMergeModePortH
#endif // (DMXNODE_PORTS == 8)
    };

    inline static constexpr json::PortKey kOutputStylePortA{"output_style_a", 14, Fnv1a32("output_style_a", 14)};
#if (DMXNODE_PORTS > 1)
    inline static constexpr json::PortKey kOutputStylePortB{"output_style_b", 14, Fnv1a32("output_style_b", 14)};
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
    inline static constexpr json::PortKey kOutputStylePortC{"output_style_c", 14, Fnv1a32("output_style_c", 14)};
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
    inline static constexpr json::PortKey kOutputStylePortD{"output_style_d", 14, Fnv1a32("output_style_d", 14)};
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
    inline static constexpr json::PortKey kOutputStylePortE{"output_style_e", 14, Fnv1a32("output_style_e", 14)};
    inline static constexpr json::PortKey kOutputStylePortF{"output_style_f", 14, Fnv1a32("output_style_f", 14)};
    inline static constexpr json::PortKey kOutputStylePortG{"output_style_g", 14, Fnv1a32("output_style_g", 14)};
    inline static constexpr json::PortKey kOutputStylePortH{"output_style_h", 14, Fnv1a32("output_style_h", 14)};
#endif // (DMXNODE_PORTS == 8)

    static constexpr json::PortKey kOutputStylePort[] = 
    {
        kOutputStylePortA,
#if (DMXNODE_PORTS > 1)
        kOutputStylePortB,
#endif // (DMXNODE_PORTS > 1)
#if (DMXNODE_PORTS > 2)
        kOutputStylePortC,
#endif // (DMXNODE_PORTS > 2)
#if (DMXNODE_PORTS > 3)
        kOutputStylePortD,
#endif // (DMXNODE_PORTS > 3)
#if (DMXNODE_PORTS == 8)
        kOutputStylePortE, 
        kOutputStylePortF, 
        kOutputStylePortG,
        kOutputStylePortH
#endif // (DMXNODE_PORTS == 8)
    };
};
} // namespace json

#endif // JSON_DMXNODEPARAMSCONST_H_
