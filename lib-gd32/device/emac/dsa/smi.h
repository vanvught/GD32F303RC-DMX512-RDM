/*
 * smi.h
 */

#ifndef SMI_H_
#define SMI_H_

#include <cstdint>

#include "gd32.h"

#include "debug.h"

/* Offset 0x00: SMI Command Register */
#define MV88E6XXX_SMI_CMD					0x00
#define MV88E6XXX_SMI_CMD_BUSY				0x8000
#define MV88E6XXX_SMI_CMD_MODE_MASK			0x1000
#define MV88E6XXX_SMI_CMD_MODE_45			0x0000
#define MV88E6XXX_SMI_CMD_MODE_22			0x1000
#define MV88E6XXX_SMI_CMD_OP_MASK			0x0c00
#define MV88E6XXX_SMI_CMD_OP_22_WRITE		0x0400
#define MV88E6XXX_SMI_CMD_OP_22_READ		0x0800
#define MV88E6XXX_SMI_CMD_OP_45_WRITE_ADDR	0x0000
#define MV88E6XXX_SMI_CMD_OP_45_WRITE_DATA	0x0400
#define MV88E6XXX_SMI_CMD_OP_45_READ_DATA	0x0800
#define MV88E6XXX_SMI_CMD_OP_45_READ_DATA_INC	0x0c00
#define MV88E6XXX_SMI_CMD_DEV_ADDR_MASK		0x003e
#define MV88E6XXX_SMI_CMD_REG_ADDR_MASK		0x001f

/* Offset 0x01: SMI Data Register */
#define MV88E6XXX_SMI_DATA					0x01

inline int mv88e6xxx_smi_direct_read(uint32_t nPhyAddress, uint32_t nPhyRegister, uint16_t& nData) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nPhyAddress=%u, nPhyRegister=0x%.4x", nPhyAddress, nPhyRegister);

	if (ERROR == enet_phy_write_read(ENET_PHY_READ, nPhyAddress, nPhyRegister, &nData)) {
		DEBUG_EXIT
		return -1;
	}

	DEBUG_PRINTF("pData=0x%.4x", nData);
	DEBUG_EXIT
	return 0;
}

inline int mv88e6xxx_smi_direct_write(uint32_t nPhyAddress, uint32_t nPhyRegister, uint16_t nData) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nPhyAddress=%u, nPhyRegister=0x%.4x, nData=0x%.4x", nPhyAddress, nPhyRegister, nData);

	if (ERROR == enet_phy_write_read(ENET_PHY_WRITE, nPhyAddress, nPhyRegister, &nData)) {
		DEBUG_EXIT
		return -1;
	}

	DEBUG_EXIT
	return 0;
}

int mv88e6xxx_smi_init();
int mv88e6xxx_smi_read(const uint32_t nDeviceAddress, uint32_t nDeviceRegister, uint16_t& nData);
int mv88e6xxx_smi_write(const uint32_t nDeviceAddress, const uint32_t nDeviceRegister, const uint16_t nData);

#endif /* SMI_H_ */
