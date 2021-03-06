/*
 * File: arch/arm/plat-omap/include/mach/dt_path.h
 *
 * Copyright (C) 2009 Motorola, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
/* Date	 Author	  Comment
 * ===========  ==============  ==============================================
 * Jun-08-2009  Motorola	Add MUX node
 * Jun-10-2009  Motorola    Add GPIO node
 */



#ifndef _MACH_DT_PATH_H
#define _MACH_DT_PATH_H
#ifdef __KERNEL__

/* Chosen */
#define DT_PATH_CHOSEN		"/Chosen@0"
#define DT_PROP_CHOSEN_BP	"bp_model"
#define DT_PROP_CHOSEN_BP_LEN	16

/* Keypad Node */
#define DT_PATH_KEYPAD		"/System@0/Keypad@0"
#define DT_PROP_KEYPAD_ROWS	"rows"
#define DT_PROP_KEYPAD_COLS	"columns"
#define DT_PROP_KEYPAD_ROWREG	"rowregister"
#define DT_PROP_KEYPAD_COLREG	"columnregister"
#define DT_PROP_KEYPAD_MAPNUM	"mapnum"
#define DT_PROP_KEYPAD_MAPS	"maps"
#define DT_PROP_KEYPAD_CLOSED_MAPS "closed_maps"
#define DT_PROP_KEYPAD_NAME	"name"

/* GPIODev Node */
#define DT_PATH_GPIOGEV		"/System@0/GPIODev@0"
#define DT_PROP_GPIODEV_INIT	"init"

/* MUX Node */
#define DT_PATH_MUX		"/System@0/IOMUX@0"
#define DT_PROP_MUX_PAD	"padinit"
#define DT_PROP_MUX_PADWKUPS	"padwkupsinit"
#define DT_PROP_MUX_OFFMODE		"offmodeinit"
#define DT_PROP_MUX_OFFMODEWKUPS	"offmodewkupsinit"

/* GPIO Node */
#define DT_PATH_GPIO        "/System@0/GPIO@0"
#define DT_PROP_GPIO_MAP    "signalmap"

/* Lighting Node */
#define DT_BACKLIGHT		"/System@0/DisplayBacklight@0"
#define DT_PROP_LCD_LED	    "lcd_led"

#define DT_KPAD_LED		"/System@0/KeypadBacklight@0"
#define DT_PROP_KPAD_LED	"kpad_led"

#define DT_NOTIFICATION_LED	"/System@0/NotificationLED@0"
#define DT_PROP_BT_LED	"bt_led"

#endif
#endif
