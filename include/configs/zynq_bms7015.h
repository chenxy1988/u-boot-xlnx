/*
 * (C) Copyright 2012 Xilinx
 * (C) Copyright 2014 Digilent Inc.
 *
 * Configuration for Zynq Development Board - ZYBO
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_MZ702A_H
#define __CONFIG_ZYNQ_MZ702A_H

/* Default environment */
#ifndef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"ethaddr=00:0a:35:00:01:22\0" \
	"fdt_high=0x20000000\0" \
	"initrd_high=0x20000000\0" \
	"kernel_image=uImage\0" \
	"kernel_load_address=0x2080000\0" \
	"ramdisk_image=uramdisk.image.gz\0" \
	"ramdisk_load_address=0x4000000\0" \
	"devicetree_image=devicetree.dtb\0" \
	"devicetree_load_address=0x2000000\0" \
	"bitstream_image=system.bit.bin\0" \
	"loadbit_addr=0x100000\0" \
	"bootenv=uEnv.txt\0" \
	"loadbootenv_addr=0x2000000\0" \
	"loadbootenv=load ${devtype} ${devnum} ${loadbootenv_addr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from ${devtype} ${devnum}...; " \
		"env import -t ${loadbootenv_addr} $filesize\0" \
	"uenvboot=run loadbootenv && echo Loaded environment from ${bootenv} && run importbootenv; " \
		"if test -n $uenvcmd; then echo Running uenvcmd ...; run uenvcmd; fi\0" \
	"fpga_progbit=load ${devtype} ${devnum} ${loadbit_addr} ${bitstream_image}\0" \
	"fpga_loadbit=if run fpga_progbit; then fpga load 0 ${loadbit_addr} ${filesize}; fi;\0" \
	"usb0_root=setenv root /dev/sda2\0" \
	"usb1_root=setenv root /dev/sdb2\0" \
	"mmc0_root=setenv root /dev/mmcblk0p2\0" \
	"mmc1_root=setenv root /dev/mmcblk1p2\0" \
	"console=console=ttyPS0,115200 earlyprintk\0" \
	"baseargs=setenv bootargs ${console} root=${root} rw rootfstype=ext4 rootwait ${optargs}\0" \
	"loadkernel=load ${devtype} ${devnum} ${kernel_load_address} ${kernel_image}\0" \
	"loaddtb=load ${devtype} ${devnum} ${devicetree_load_address} ${devicetree_image}\0" \
	"loadinitrd=load ${devtype} ${devnum} ${ramdisk_load_address} ${ramdisk_image}\0" \
	"tryboot=bootm ${kernel_load_address} ${initrd_load_address} ${devicetree_load_address};\0" \
	"check_ramdisk= " \
		"if run loadinitrd; then " \
			"setenv root /dev/ram; setenv initrd_load_address ${ramdisk_load_address}; " \
		"else " \
			"run ${devtype}${devnum}_root; setenv initrd_load_address -; " \
		"fi;\0" \
	"scanusb=setenv devtype usb && setenv devnum 0 && usb reset && usb start && usb storage && run scandev; \0" \
	"scanmmc=setenv devtype mmc; setenv devnum 0; run scandev; setenv devnum 1; run scandev;\0" \
	"emmcboot=setenv devtype mmc; setenv devnum 1; run scandev \0" \
	"qspiboot=echo QSPI Boot Mode; run scanusb || run emmcboot \0" \
	"sdboot=echo SDCARD Boot Mode; prefix; setenv devtype mmc; run scandev \0" \
	"scandev=echo [INFO] Scanning ${devtype} ${devnum}...; && " \
		"if $devtype dev $devnum; then " \
                        "run uenvboot; " \
			"echo [INFO] Trying to boot from $devtype $devnum; " \
			"run loadkernel && run loaddtb && run fpga_loadbit; " \
			"run check_ramdisk && run baseargs && echo ${bootargs} && run tryboot; sleep 5; reset;" \
		"fi; \0" \
		BOOTENV
#endif


#include <configs/zynq-common.h>

#endif /* __CONFIG_ZYNQ_MZ702A_H */
