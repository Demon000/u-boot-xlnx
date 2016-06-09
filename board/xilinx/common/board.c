// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2014 - 2019 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 */

#include <common.h>
#include <dm/uclass.h>
#include <i2c.h>
#include <spi.h>
#include <spi_flash.h>

int zynq_board_read_rom_ethaddr(unsigned char *ethaddr)
{
	int ret = -EINVAL;

#if defined(CONFIG_ZYNQ_GEM_I2C_MAC_OFFSET)
	struct udevice *dev;
	ofnode eeprom;

	eeprom = ofnode_get_chosen_node("xlnx,eeprom");
	if (!ofnode_valid(eeprom))
		return -ENODEV;

	debug("%s: Path to EEPROM %s\n", __func__,
	      ofnode_get_chosen_prop("xlnx,eeprom"));

	ret = uclass_get_device_by_ofnode(UCLASS_I2C_EEPROM, eeprom, &dev);
	if (ret)
		return ret;

	ret = dm_i2c_read(dev, CONFIG_ZYNQ_GEM_I2C_MAC_OFFSET, ethaddr, 6);
	if (ret)
		debug("%s: I2C EEPROM MAC address read failed\n", __func__);
	else
		debug("%s: I2C EEPROM MAC %pM\n", __func__, ethaddr);
#endif

#if defined(CONFIG_ZYNQ_GEM_SPI_MAC_OFFSET)
#define CMD_OTPREAD_ARRAY_FAST		0x4b
	struct spi_flash *flash;
	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);

	if (!flash) {
		printf("SPI(bus:%u cs:%u) probe failed\n",
			CONFIG_SF_DEFAULT_BUS,
			CONFIG_SF_DEFAULT_CS);
		return 0;
	}

	/*
	 * Set the cmd to otp read
	 */
	flash->read_cmd = CMD_OTPREAD_ARRAY_FAST;
	if (spi_flash_read(flash, CONFIG_ZYNQ_GEM_SPI_MAC_OFFSET, 6, ethaddr))
		printf("SPI MAC address read failed\n");

	if (flash)
		spi_flash_free(flash);

#endif

	return ret;
}
