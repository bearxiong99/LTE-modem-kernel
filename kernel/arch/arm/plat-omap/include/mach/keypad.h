/*
 *  arch/arm/plat-omap/include/mach/keypad.h
 *
 *  Copyright (C) 2006 Komal Shah <komal_shah802003@yahoo.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef ASMARM_ARCH_KEYPAD_H
#define ASMARM_ARCH_KEYPAD_H

struct omap_kp_switchmap {
	unsigned int gpio;
	unsigned int key;
};

struct omap_kp_platform_data {
	int rows;
	int cols;
	const unsigned short *keymap;
	int irq;
	unsigned int keymapsize;
	struct omap_kp_switchmap *switchmap;
	unsigned int switchmapsize;
	unsigned int rep:1;
	unsigned int dbounce:1;
    unsigned long   dbounce_delay;
	unsigned long delay;
	char *input_name;
	char *input_phys_device;
	/* specific to OMAP242x*/
	unsigned int *row_gpios;
	unsigned int *col_gpios;
};

/* Group (0..3) -- when multiple keys are pressed, only the
 * keys pressed in the same group are considered as pressed. This is
 * in order to workaround certain crappy HW designs that produce ghost
 * keypresses. */
#define GROUP_0		(0 << 16)
#define GROUP_1		(1 << 16)
#define GROUP_2		(2 << 16)
#define GROUP_3		(3 << 16)
#define GROUP_MASK	GROUP_3

#define ROWCOL_MASK		0xFF000000
#define KEY_PERSISTENT		0x00800000
#define KEYNUM_MASK		0x00EFFFFF
#define KEY(col, row, val) (((col) << 28) | ((row) << 24) | (val))
#define PERSISTENT_KEY(col, row) (((col) << 28) | ((row) << 24) | \
						KEY_PERSISTENT)

#endif

