/*
 * ehci-omap.c - driver for USBHOST on OMAP 34xx processor
 *
 * Bus Glue for OMAP34xx USBHOST 3 port EHCI controller
 * Tested on OMAP3430 ES2.0 SDP
 *
 * Copyright (C) 2007-2008 Texas Instruments, Inc.
 * Copyright (C) 2008-2009 Motorola, Inc.
 * Author: Vikram Pandita <vikram.pandita@ti.com>
 *
 * Based on "ehci-fsl.c" and "ehci-au1xxx.c" ehci glue layers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Revision History:
 *
 * Date        Author          Comment
 * ----------  --------------- -------------------------------------------
 * 03/25/2008  Motorola        Porting USB feature for OMAP chip
 * 03/25/2009  Motorola        Conditionally configure port for OHCI
 * 04/24/2008  Motorola        Update GPIO common API
 */

#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/clk.h>
#include <linux/usb/omap.h>
#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
#include <linux/device.h>
#endif
#include <mach/gpio.h>

#include "ehci-omap.h"
#include <mach/hardware.h>

#ifdef CONFIG_MOT_FEAT_GPIO_API
#include <mach/mot-gpio-omap.h>
#endif

struct usb_hcd *ghcd;
#ifdef CONFIG_OMAP_EHCI_PHY_MODE
/* EHCI connected to External PHY */

/* External USB connectivity board: 750-2099-001(C)
 * Connected to OMAP3430 SDP
 * The board has Port1 and Port2 connected to ISP1504 in 12-pin ULPI mode
 */

/* ISSUE1:
 *      ISP1504 for input clocking mode needs special reset handling
 *	Hold the PHY in reset by asserting RESET_N signal
 * 	Then start the 60Mhz clock input to PHY
 * 	Release the reset after a delay -
 * 		to get the PHY state machine in working state
 */
#define EXTERNAL_PHY_RESET
#define	EXT_PHY_RESET_GPIO_PORT1	(57)
#define	EXT_PHY_RESET_GPIO_PORT2	(61)
#define	EXT_PHY_RESET_DELAY		(500)
#ifdef CONFIG_MACH_HPM1200L
	#define EXT_PHY_RESET_GPIO_PORT (149)
#endif

#endif /* CONFIG_OMAP_EHCI_PHY_MODE */

/*-------------------------------------------------------------------------*/

/* Define USBHOST clocks for clock management */
struct ehci_omap_clock_defs {
	struct clk	*usbhost_ick_clk;
	struct clk	*usbhost2_120m_fck_clk;
	struct clk	*usbhost1_48m_fck_clk;
	struct clk	*usbtll_fck_clk;
	struct clk	*usbtll_ick_clk;
	unsigned	suspended:1;
};

static int usbtll_fclk_enabled;
static int usbhost_fclk_enabled;
static DEFINE_SPINLOCK(usbtll_clock_lock);

/* Clock names as per clock framework: May change so keep as #defs */
#define USBHOST_ICKL	"usbhost_ick"
#define USBHOST_120M_FCLK	"usbhost_120m_fck"
#define USBHOST_48M_FCLK	"usbhost_48m_fck"
#define USBHOST_TLL_ICKL	"usbtll_ick"
#define USBHOST_TLL_FCLK	"usbtll_fck"
/*-------------------------------------------------------------------------*/

#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
unsigned short disable_irq_value;
static ssize_t disable_irq_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t disable_irq_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n);
static struct kobj_attribute usbtll_disable_irq_attr = __ATTR(disable_irq_value, 0644, disable_irq_show, disable_irq_store);

static ssize_t disable_irq_show(struct kobject *kobj, struct kobj_attribute *attr,
                                char *buf)
{
        printk("in disable_irq_show()\n");
        return sprintf(buf, "%hu\n", disable_irq_value);
}
static ssize_t disable_irq_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
        unsigned short value;

        if (sscanf(buf, "%hu", &value) != 1 ||
                (value != 0 && value != 1)) {
                        printk(KERN_ERR "disable_irq_store: Invalid value\n");
                        return -EINVAL;
        }


        if (attr == &usbtll_disable_irq_attr)
            disable_irq_value = value;

        printk("disable_irq_value=%d\n", disable_irq_value);
        if (disable_irq_value)
        {
                //omap_writel(6, 0x48062018);
                //omap_writel(0, 0x4806201C);
                printk("disable usbtll irq in disable_irq_store()\n");
        }

        return n;
}
#endif

#ifndef CONFIG_OMAP_EHCI_PHY_MODE

static void omap_usb_utmi_init(struct usb_hcd *hcd, u8 tll_channel_mask)
{
	int i;

	/* Use UTMI Ports of TLL */
	omap_writel((1 << OMAP_UHH_HOSTCONFIG_P1_ULPI_BYPASS_SHIFT)|
			(1 << OMAP_UHH_HOSTCONFIG_P3_ULPI_BYPASS_SHIFT)|
			(1<<OMAP_UHH_HOSTCONFIG_INCR4_BURST_EN_SHIFT)|
			(1<<OMAP_UHH_HOSTCONFIG_INCR8_BURST_EN_SHIFT)|
			(1<<OMAP_UHH_HOSTCONFIG_INCR16_BURST_EN_SHIFT)|
			(0<<OMAP_UHH_HOSTCONFIG_INCRX_ALIGN_EN_SHIFT),
						OMAP_UHH_HOSTCONFIG);
	/* Enusre bit is set */
	while (!(omap_readl(OMAP_UHH_HOSTCONFIG) &
		 ((1 << OMAP_UHH_HOSTCONFIG_P1_ULPI_BYPASS_SHIFT) | (1 << OMAP_UHH_HOSTCONFIG_P3_ULPI_BYPASS_SHIFT))))
		cpu_relax();

	dev_dbg(hcd->self.controller, "\nEntered UTMI MODE: success\n");

	/* Program the 3 TLL channels upfront */

	for (i = 0; i < OMAP_TLL_CHANNEL_COUNT; i++) {

		/* Disable AutoIdle */
		/* Changed to Enable AutoIdle of UTMI
		 * This will allow TLL FCLK END interrupt to be generated*/
		omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) |
			    (1 << OMAP_TLL_CHANNEL_CONF_UTMIAUTOIDLE_SHIFT),
			    OMAP_TLL_CHANNEL_CONF(i));

		/* Disable BitStuffing */
		omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) |
			(1<<OMAP_TLL_CHANNEL_CONF_ULPINOBITSTUFF_SHIFT),
			OMAP_TLL_CHANNEL_CONF(i));

		/* SDR Mode */
		omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) &
			    ~(1<<OMAP_TLL_CHANNEL_CONF_ULPIDDRMODE_SHIFT),
			    OMAP_TLL_CHANNEL_CONF(i));

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
		if ((is_cdma_phone()) && (i == 2)) {
			/* 0x1: UTMI-to-serial (FS/LS) mode:
			 * To serial controller (TLL) or serial PHY */
			omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) |
				(1<<OMAP_TLL_CHANNEL_CONF_CHANMODE_SHIFT),
				OMAP_TLL_CHANNEL_CONF(i));
			/* 0x3: "4-pin" bidirectional PHY i/f mode. */
			omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) |
				(3<<OMAP_TLL_CHANNEL_CONF_FSLSMODE_SHIFT),
				OMAP_TLL_CHANNEL_CONF(i));
		}
#endif /* CONFIG_USB_OHCI_HCD or CONFIG_USB_OHCI_HCD_MODULE */

	}

	/* Program Common TLL register */
	omap_writel((1 << OMAP_TLL_SHARED_CONF_FCLK_IS_ON_SHIFT) |
			(1 << OMAP_TLL_SHARED_CONF_USB_DIVRATION_SHIFT) |
			(0 << OMAP_TLL_SHARED_CONF_USB_180D_SDR_EN_SHIFT) |
			(0 << OMAP_TLL_SHARED_CONF_USB_90D_DDR_EN_SHFT),
				OMAP_TLL_SHARED_CONF);

	/* Enable channels now */
	for (i = 0; i < OMAP_TLL_CHANNEL_COUNT; i++) {

		/* Enable only the channel that is needed */
		if (!(tll_channel_mask & 1<<i))
			continue;

		omap_writel(omap_readl(OMAP_TLL_CHANNEL_CONF(i)) |
			    (1<<OMAP_TLL_CHANNEL_CONF_CHANEN_SHIFT),
			    OMAP_TLL_CHANNEL_CONF(i));

		omap_writeb(0xBE, OMAP_TLL_ULPI_SCRATCH_REGISTER(i));
		dev_dbg(hcd->self.controller, "\nULPI_SCRATCH_REG[ch=%d]"
			"= 0x%02x\n",
			i+1, omap_readb(OMAP_TLL_ULPI_SCRATCH_REGISTER(i)));
	}
}

#else
# define omap_usb_utmi_init(x, y)	0
#endif


/* omap_start_ehc
 * 	- Start the TI USBHOST controller
 */
static int omap_start_ehc(struct platform_device *dev, struct usb_hcd *hcd)
{
	struct ehci_omap_clock_defs *ehci_clocks;

	dev_dbg(hcd->self.controller, "starting TI EHCI USB Controller\n");

	ehci_clocks = (struct ehci_omap_clock_defs *)(
				((char *)hcd_to_ehci(hcd)) +
					sizeof(struct ehci_hcd));

	/* Enable Clocks for USBHOST */
	ehci_clocks->usbhost_ick_clk = clk_get(&dev->dev,
						USBHOST_ICKL);
	if (IS_ERR(ehci_clocks->usbhost_ick_clk))
		return PTR_ERR(ehci_clocks->usbhost_ick_clk);
	clk_enable(ehci_clocks->usbhost_ick_clk);


	ehci_clocks->usbhost2_120m_fck_clk = clk_get(&dev->dev,
							USBHOST_120M_FCLK);
	if (IS_ERR(ehci_clocks->usbhost2_120m_fck_clk)) {
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_put(ehci_clocks->usbhost_ick_clk);
		return PTR_ERR(ehci_clocks->usbhost2_120m_fck_clk);
	}
	clk_enable(ehci_clocks->usbhost2_120m_fck_clk);

	ehci_clocks->usbhost1_48m_fck_clk = clk_get(&dev->dev,
						USBHOST_48M_FCLK);
	if (IS_ERR(ehci_clocks->usbhost1_48m_fck_clk)) {
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_put(ehci_clocks->usbhost_ick_clk);
		clk_disable(ehci_clocks->usbhost2_120m_fck_clk);
		clk_put(ehci_clocks->usbhost2_120m_fck_clk);
		return PTR_ERR(ehci_clocks->usbhost1_48m_fck_clk);
	}
	clk_enable(ehci_clocks->usbhost1_48m_fck_clk);


#ifdef EXTERNAL_PHY_RESET
	/* Refer: ISSUE1 */
#ifndef CONFIG_MACH_HPM1200L
	omap_request_gpio(EXT_PHY_RESET_GPIO_PORT1);
	omap_set_gpio_direction(EXT_PHY_RESET_GPIO_PORT1, 0);
	omap_request_gpio(EXT_PHY_RESET_GPIO_PORT2);
	omap_set_gpio_direction(EXT_PHY_RESET_GPIO_PORT2, 0);
	omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT1, 1);
	omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT2, 1);
#elif defined(CONFIG_MOT_FEAT_GPIO_API)
        if (gpio_request(GPIO_SIGNAL_USB_IPC_RESET_PHY, NULL) < 0)
            printk(KERN_ERR "can't get EXT_PHY_RESET_GPIO_PORT pin.\n");
        if (gpio_direction_output(GPIO_SIGNAL_USB_IPC_RESET_PHY, 0) != 0)
            printk(KERN_ERR "can't set EXT_PHY_RESET_GPIO_PORT pin direction output LOW.\n");
#else
	omap_request_gpio(EXT_PHY_RESET_GPIO_PORT);
	omap_set_gpio_direction(EXT_PHY_RESET_GPIO_PORT, 0);
	omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT, 0);
#endif
	/* Hold the PHY in RESET for enough time till DIR is high */
	udelay(EXT_PHY_RESET_DELAY);
#endif

	/* Configure TLL for 60Mhz clk for ULPI */
	ehci_clocks->usbtll_fck_clk = clk_get(&dev->dev, USBHOST_TLL_FCLK);
	if (IS_ERR(ehci_clocks->usbtll_fck_clk)) {
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_put(ehci_clocks->usbhost_ick_clk);
		clk_disable(ehci_clocks->usbhost2_120m_fck_clk);
		clk_put(ehci_clocks->usbhost2_120m_fck_clk);
		clk_disable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_put(ehci_clocks->usbhost1_48m_fck_clk);
		return PTR_ERR(ehci_clocks->usbtll_fck_clk);
	}
	clk_enable(ehci_clocks->usbtll_fck_clk);

	ehci_clocks->usbtll_ick_clk = clk_get(&dev->dev, USBHOST_TLL_ICKL);
	if (IS_ERR(ehci_clocks->usbtll_ick_clk)) {
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_put(ehci_clocks->usbhost_ick_clk);
		clk_disable(ehci_clocks->usbhost2_120m_fck_clk);
		clk_put(ehci_clocks->usbhost2_120m_fck_clk);
		clk_disable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_put(ehci_clocks->usbhost1_48m_fck_clk);
		clk_disable(ehci_clocks->usbtll_fck_clk);
		clk_put(ehci_clocks->usbtll_fck_clk);
		return PTR_ERR(ehci_clocks->usbtll_ick_clk);
	}
	clk_enable(ehci_clocks->usbtll_ick_clk);

	usbtll_fclk_enabled = 1;
	usbhost_fclk_enabled = 1;

	ehci_clocks->suspended = 0; /* Superfluous! */

	/* Disable Auto Idle of USBTLL */
	/* Changed to enable autoidle so that core can transition
	 * automatically */
	cm_write_mod_reg((1 << OMAP3430ES2_AUTO_USBTLL_SHIFT),
				CORE_MOD, CM_AUTOIDLE3);

	/* Wait for TLL to be Active */
	while ((cm_read_mod_reg(CORE_MOD, OMAP2430_CM_IDLEST3)
			& (1 << OMAP3430ES2_ST_USBTLL_SHIFT)))
		cpu_relax();

	/* perform TLL soft reset, and wait until reset is complete */
	omap_writel(1 << OMAP_USBTLL_SYSCONFIG_SOFTRESET_SHIFT,
			OMAP_USBTLL_SYSCONFIG);
	/* Wait for TLL reset to complete */
	while (!(omap_readl(OMAP_USBTLL_SYSSTATUS)
			& (1 << OMAP_USBTLL_SYSSTATUS_RESETDONE_SHIFT)))
		cpu_relax();

	dev_dbg(hcd->self.controller, "TLL RESET DONE\n");

	/* Smart Idle mode */
	omap_writel((1 << OMAP_USBTLL_SYSCONFIG_ENAWAKEUP_SHIFT)     |
			(2 << OMAP_USBTLL_SYSCONFIG_SIDLEMODE_SHIFT) |
			(0 << OMAP_USBTLL_SYSCONFIG_CACTIVITY_SHIFT) |
			(1 << OMAP_USBTLL_SYSCONFIG_AUTOIDLE_SHIFT),
			OMAP_USBTLL_SYSCONFIG);

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	if (is_cdma_phone()) {
		/* Put UHH in NoIdle/NoStandby mode */
		omap_writel((0 << OMAP_UHH_SYSCONFIG_AUTOIDLE_SHIFT) |
			(1 << OMAP_UHH_SYSCONFIG_ENAWAKEUP_SHIFT) |
			(1 << OMAP_UHH_SYSCONFIG_SIDLEMODE_SHIFT) |
			(1 << OMAP_UHH_SYSCONFIG_CACTIVITY_SHIFT) |
			(1 << OMAP_UHH_SYSCONFIG_MIDLEMODE_SHIFT),
			OMAP_UHH_SYSCONFIG);
	} else {
#endif
		/* Put UHH in SmartIdle/SmartStandby mode */
		omap_writel((1 << OMAP_UHH_SYSCONFIG_AUTOIDLE_SHIFT) |
			(1 << OMAP_UHH_SYSCONFIG_ENAWAKEUP_SHIFT) |
			(2 << OMAP_UHH_SYSCONFIG_SIDLEMODE_SHIFT) |
			(0 << OMAP_UHH_SYSCONFIG_CACTIVITY_SHIFT) |
			(2 << OMAP_UHH_SYSCONFIG_MIDLEMODE_SHIFT),
			OMAP_UHH_SYSCONFIG);
#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	}
#endif


#ifdef CONFIG_OMAP_EHCI_PHY_MODE
	/* Bypass the TLL module for PHY mode operation */
	omap_writel((0 << OMAP_UHH_HOSTCONFIG_P1_ULPI_BYPASS_SHIFT)|
						(0 << OMAP_UHH_HOSTCONFIG_P2_ULPI_BYPASS_SHIFT)
						(1<<OMAP_UHH_HOSTCONFIG_INCR4_BURST_EN_SHIFT)|
						(1<<OMAP_UHH_HOSTCONFIG_INCR8_BURST_EN_SHIFT)|
						(1<<OMAP_UHH_HOSTCONFIG_INCR16_BURST_EN_SHIFT)|
						(0<<OMAP_UHH_HOSTCONFIG_INCRX_ALIGN_EN_SHIFT),
						OMAP_UHH_HOSTCONFIG);
	/* Ensure that BYPASS is set */
	while (omap_readl(OMAP_UHH_HOSTCONFIG) &
	       ((1 << OMAP_UHH_HOSTCONFIG_P1_ULPI_BYPASS_SHIFT) | (1 << OMAP_UHH_HOSTCONFIG_P2_ULPI_BYPASS_SHIFT)))
		cpu_relax();

	dev_dbg(hcd->self.controller, "Entered ULPI PHY MODE: success\n");

#else
	/* Enable UTMI mode for all 3 TLL channels */
	omap_usb_utmi_init(hcd,
		OMAP_TLL_CHANNEL_3_EN_MASK
		);
#endif

#ifdef EXTERNAL_PHY_RESET
	/* Refer ISSUE1:
	 * Hold the PHY in RESET for enough time till PHY is settled and ready
	 */
	udelay(EXT_PHY_RESET_DELAY);
#ifndef CONFIG_MACH_HPM1200L
	omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT1, 0);
	omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT2, 0);
#elif defined(CONFIG_MOT_FEAT_GPIO_API)
        gpio_set_value(GPIO_SIGNAL_USB_IPC_RESET_PHY, 1);
#else
        omap_set_gpio_dataout(EXT_PHY_RESET_GPIO_PORT, 1);
#endif /* CONFIG_MACH_HPM1200L */
#endif

#if defined(CONFIG_MACH_HPM1200L)
	/* Refer ISSUE2: LINK assumes external charge pump */

	/* use Port1 VBUS to charge externally Port2:
	 * 	So for PHY mode operation use Port2 only
	 */
        omap_writel((0xA << EHCI_INSNREG05_ULPI_REGADD_SHIFT) |/* OTG ctrl reg*/
                        (2 << EHCI_INSNREG05_ULPI_OPSEL_SHIFT) |/*   Write */
                        (2 << EHCI_INSNREG05_ULPI_PORTSEL_SHIFT) |/* Port1 */
                        (1 << EHCI_INSNREG05_ULPI_CONTROL_SHIFT) |/* Start */
                        (0x26),
                        EHCI_INSNREG05_ULPI);

	while (!(omap_readl(EHCI_INSNREG05_ULPI) &
		(1<<EHCI_INSNREG05_ULPI_CONTROL_SHIFT)));
#endif /* CONFIG_MACH_HPM1200L */

        return 0;
}

/*-------------------------------------------------------------------------*/

static void omap_stop_ehc(struct platform_device *dev, struct usb_hcd *hcd)
{
	struct ehci_omap_clock_defs *ehci_clocks;

	ehci_clocks = (struct ehci_omap_clock_defs *)
			(((char *)hcd_to_ehci(hcd)) + sizeof(struct ehci_hcd));

	dev_dbg(hcd->self.controller, "stopping TI EHCI USB Controller\n");

	/* Reset OMAP modules for insmod/rmmod to work */
	omap_writel((1<<1), OMAP_UHH_SYSCONFIG);
	while (!(omap_readl(OMAP_UHH_SYSSTATUS) & (1<<0)))
		cpu_relax();
	while (!(omap_readl(OMAP_UHH_SYSSTATUS) & (1<<1)))
		cpu_relax();
	while (!(omap_readl(OMAP_UHH_SYSSTATUS) & (1<<2)))
		cpu_relax();
	dev_dbg(hcd->self.controller,
		"UHH RESET DONE OMAP_UHH_SYSSTATUS %x !!\n",
			omap_readl(OMAP_UHH_SYSSTATUS));

	omap_writel((1<<1), OMAP_USBTLL_SYSCONFIG);
	while (!(omap_readl(OMAP_USBTLL_SYSSTATUS) & (1<<0)))
		cpu_relax();
	dev_dbg(hcd->self.controller, "TLL RESET DONE\n");

	if (ehci_clocks->usbtll_fck_clk != NULL) {
		clk_disable(ehci_clocks->usbtll_fck_clk);
		clk_put(ehci_clocks->usbtll_fck_clk);
		ehci_clocks->usbtll_fck_clk = NULL;
	}

	usbtll_fclk_enabled = 0;

	if (ehci_clocks->usbhost_ick_clk != NULL) {
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_put(ehci_clocks->usbhost_ick_clk);
		ehci_clocks->usbhost_ick_clk = NULL;
	}

	if (ehci_clocks->usbhost1_48m_fck_clk != NULL) {
		clk_disable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_put(ehci_clocks->usbhost1_48m_fck_clk);
		ehci_clocks->usbhost1_48m_fck_clk = NULL;
	}

	if (ehci_clocks->usbhost2_120m_fck_clk != NULL) {
		clk_disable(ehci_clocks->usbhost2_120m_fck_clk);
		clk_put(ehci_clocks->usbhost2_120m_fck_clk);
		ehci_clocks->usbhost2_120m_fck_clk = NULL;
	}

	if (ehci_clocks->usbtll_ick_clk != NULL) {
		clk_disable(ehci_clocks->usbtll_ick_clk);
		clk_put(ehci_clocks->usbtll_ick_clk);
		ehci_clocks->usbtll_ick_clk = NULL;
	}


#ifdef EXTERNAL_PHY_RESET
	omap_free_gpio(EXT_PHY_RESET_GPIO_PORT1);
	omap_free_gpio(EXT_PHY_RESET_GPIO_PORT2);
#endif

	dev_dbg(hcd->self.controller,
		"Clock to USB host has been disabled\n");
}

static const struct hc_driver ehci_omap_hc_driver;

/*-------------------------------------------------------------------------*/

#ifdef CONFIG_PM
#define OHCI_BASE_ADDR 0x48064400
#define OHCI_HC_CONTROL		(OHCI_BASE_ADDR + 0x4)
#define OHCI_HC_CTRL_SUSPEND	(3 << 6)
#define OHCI_HC_CTRL_RESUME	(1 << 6)

static int omap_ehci_bus_suspend(struct usb_hcd *hcd)
{
	struct ehci_omap_clock_defs *ehci_clocks;
	int ret = 0;
	unsigned long flags;
	u32 status;
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);

#if defined(CONFIG_ARCH_OMAP34XX)
	int res = 0;
	struct omap_usb_platform_data *config = hcd->self.controller->platform_data;
#endif
	/* mask interrupt 77 to avoid race condition with ehci_irq */
	omap_writel(0x2000, 0x482000cc);

	ehci_clocks = (struct ehci_omap_clock_defs *)
			(((char *)hcd_to_ehci(hcd)) + sizeof(struct ehci_hcd));
	ret = ehci_bus_suspend(hcd);
	if (ret < 0)
		goto end;

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	if (!is_cdma_phone() && !ehci_clocks->suspended) {
#else
	if (!ehci_clocks->suspended) {
#endif
		/* We need to suspend OHCI as well, before the domain
		 * can transition
		 */
		omap_writel(OHCI_HC_CTRL_SUSPEND, OHCI_HC_CONTROL);
		mdelay(8); /* MSTANDBY assertion is delayed by ~8ms */

#if defined(CONFIG_ARCH_OMAP34XX)
		if (config->usbhost_standby_status)
			res = config->usbhost_standby_status();

		if (res == 0) {
			printk(KERN_ERR "ehci: suspend failed!\n");
			ehci_bus_resume(hcd);
			ret = -EBUSY;
			goto end;
		}
#endif

		/* Ports suspended: Stop All Clks */
#if 0
		/* ICKs are Autoidled. No need for explicit control*/
		clk_disable(ehci_clocks->usbhost_ick_clk);
		clk_disable(ehci_clocks->usbtll_ick_clk);
#endif
		spin_lock_irqsave(&usbtll_clock_lock, flags);
		status = ehci_readl(ehci, &ehci->regs->status);
		if (status & INTR_MASK) {
			/* pending irq, resume */
			ret = -EBUSY;
			goto resume;
		}
		omap_writel(omap_readl(OMAP_TLL_SHARED_CONF) & ~(1),
				OMAP_TLL_SHARED_CONF);
/* Enable the interrupt so that the remote-wakeup can be detected */
#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
                if (disable_irq_value)
                  {
                    omap_writel(6, OMAP_USBTLL_IRQSTATUS);
                    omap_writel(0, OMAP_USBTLL_IRQENABLE);
                    printk("disable usbtll irq in omap_ehci_bus_suspend()\n");
                  }
                else
                  {
#endif
                    omap_writel(7, OMAP_USBTLL_IRQSTATUS);
                    omap_writel(1, OMAP_USBTLL_IRQENABLE);
#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
                  }
#endif
		ehci_clocks->suspended = 1;
		usbtll_fclk_enabled = 0;
		clk_disable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_disable(ehci_clocks->usbhost2_120m_fck_clk);
		clk_disable(ehci_clocks->usbtll_fck_clk);
		spin_unlock_irqrestore(&usbtll_clock_lock, flags);
	}
#ifdef CONFIG_HAS_WAKELOCK
	wake_unlock(&ehci->wake_lock_ehci_pm);
#endif
	goto end;

resume:
	omap_writel(OHCI_HC_CTRL_RESUME, OHCI_HC_CONTROL);
	spin_unlock_irqrestore(&usbtll_clock_lock, flags);
	ehci_bus_resume(hcd);

end:
	/* unmask irq 77 */
	omap_writel(0x2000, 0x482000c8);
	return ret;
}

static int omap_ehci_bus_resume(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci;
	struct ehci_omap_clock_defs *ehci_clocks;
	int ret = 0;
	unsigned long flags;

	ehci = hcd_to_ehci(hcd);
	ehci_clocks = (struct ehci_omap_clock_defs *)
			(((char *)hcd_to_ehci(hcd)) + sizeof(struct ehci_hcd));
#ifdef CONFIG_HAS_WAKELOCK
	wake_lock(&ehci->wake_lock_ehci_pm);
#endif
	spin_lock_irqsave(&usbtll_clock_lock, flags);

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	if (!is_cdma_phone() && ehci_clocks->suspended) {
#else
	if (ehci_clocks->suspended) {
#endif
		/* Enable clks before accessing the controller */
#if 0
		/* ICLKs are Autoidled. No need for explicit control */
		clk_enable(ehci_clocks->usbtll_ick_clk);
		clk_enable(ehci_clocks->usbhost_ick_clk);
#endif
/* If the host initiated this resume, then the TLL handler may not get called
 * so the clock will need to be turned on explicitly
 */
		if (!usbtll_fclk_enabled) {
			clk_enable(ehci_clocks->usbtll_fck_clk);
			/* Disable usbtll irq to prevent race condition */
			omap_writel(7, OMAP_USBTLL_IRQSTATUS);
			omap_writel(0, OMAP_USBTLL_IRQENABLE);
			omap_writel(1 | omap_readl(OMAP_TLL_SHARED_CONF),
					OMAP_TLL_SHARED_CONF);
			usbtll_fclk_enabled = 1;
			clk_enable(ehci_clocks->usbhost2_120m_fck_clk);
			clk_enable(ehci_clocks->usbhost1_48m_fck_clk);
			omap_writel(OHCI_HC_CTRL_RESUME, OHCI_HC_CONTROL);
			ehci_clocks->suspended = 0;
		}
	}
	spin_unlock_irqrestore(&usbtll_clock_lock, flags);
	/* Wakeup ports by resume */
	ret = ehci_bus_resume(hcd);

	return ret;
}
static void omap_ehci_shutdown(struct usb_hcd *hcd)
{
	struct ehci_omap_clock_defs *ehci_clocks;
	unsigned long flags;

	ehci_clocks = (struct ehci_omap_clock_defs *)
			((char *)hcd_to_ehci(hcd) + sizeof(struct ehci_hcd));
	spin_lock_irqsave(&usbtll_clock_lock, flags);

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	if (!is_cdma_phone() && ehci_clocks->suspended) {
#else
	if (ehci_clocks->suspended) {
#endif

#if 0
		/* ICLKs are Autoidled. No need for explicit control */
		clk_enable(ehci_clocks->usbtll_ick_clk);
		clk_enable(ehci_clocks->usbhost_ick_clk);
#endif
		if (!usbtll_fclk_enabled) {
			clk_enable(ehci_clocks->usbtll_fck_clk);
			usbtll_fclk_enabled = 1;
		}
		clk_enable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_enable(ehci_clocks->usbhost2_120m_fck_clk);
		ehci_clocks->suspended = 0;
	}
	spin_unlock_irqrestore(&usbtll_clock_lock, flags);
	ehci_shutdown(hcd);
}

#endif
/*-------------------------------------------------------------------------*/

static const struct hc_driver ehci_omap_hc_driver = {
	.description = hcd_name,
	.product_desc = "OMAP-EHCI Host Controller",
	.hcd_priv_size = sizeof(struct ehci_hcd)
				+ sizeof(struct ehci_omap_clock_defs),

	/*
	 * generic hardware linkage
	 */
	.irq = ehci_irq,
	.flags = HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 */
	.reset = ehci_init,
	.start = ehci_run,
	.stop = ehci_stop,
#ifdef CONFIG_PM
	.shutdown = omap_ehci_shutdown,
#else
	.shutdown = ehci_shutdown,
#endif

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue = ehci_urb_enqueue,
	.urb_dequeue = ehci_urb_dequeue,
	.endpoint_disable = ehci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number = ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data = ehci_hub_status_data,
	.hub_control = ehci_hub_control,
#ifdef	CONFIG_PM
	.bus_suspend = omap_ehci_bus_suspend,
	.bus_resume = omap_ehci_bus_resume,
#endif
	.relinquish_port = ehci_relinquish_port,
	.port_handed_over = ehci_port_handed_over,
};

static irqreturn_t usbtll_irq(int irq, void *tll)
{
	u32 usbtll_irqstatus;
	struct ehci_omap_clock_defs *ehci_clocks;
	struct ehci_hcd *ehci = hcd_to_ehci(ghcd);
	ehci_clocks = (struct ehci_omap_clock_defs *)
			((char *)hcd_to_ehci(ghcd) + sizeof(struct ehci_hcd));

	usbtll_irqstatus = omap_readl(OMAP_USBTLL_IRQSTATUS);

	if (usbtll_irqstatus & 1) {
#ifdef CONFIG_HAS_WAKELOCK
		wake_lock_timeout(&ehci->wake_lock_ehci_rwu, HZ/2);
#endif
		clk_enable(ehci_clocks->usbtll_fck_clk);
		usbtll_fclk_enabled = 1;
		/* Disable usbtll irq to prevent race condition in suspend */
		omap_writel(1 | omap_readl(OMAP_TLL_SHARED_CONF),
				OMAP_TLL_SHARED_CONF);
		omap_writel(usbtll_irqstatus, OMAP_USBTLL_IRQSTATUS);
		clk_enable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_enable(ehci_clocks->usbhost2_120m_fck_clk);
		omap_writel(OHCI_HC_CTRL_RESUME, OHCI_HC_CONTROL);
		ehci_clocks->suspended = 0;
		omap_writel(0, OMAP_USBTLL_IRQENABLE);
	}

	return IRQ_HANDLED;
}
/*-------------------------------------------------------------------------*/
/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */

/**
 * ehci_hcd_omap_drv_probe - initialize TI-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */
static int ehci_hcd_omap_drv_probe(struct platform_device *dev)
{
	int retval = 0;
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;

	dev_dbg(&dev->dev, "ehci_hcd_omap_drv_probe()\n");

	if (usb_disabled())
		return -ENODEV;

	retval = request_irq(78, usbtll_irq, IRQF_DISABLED | IRQF_SHARED,
			"usbtll", &dev->dev);

	if (retval < 0) {
		printk(KERN_ERR "\nCan't get USBTLL IRQ\n");
		return -ENODEV;
	}
	if (dev->resource[1].flags != IORESOURCE_IRQ) {
		dev_dbg(&dev->dev, "resource[1] is not IORESOURCE_IRQ\n");
		retval = -ENOMEM;
	}
#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
        retval = sysfs_create_file(power_kobj, &usbtll_disable_irq_attr.attr);
        if (retval)
          printk("create usbtll_disable_irq_attr failed\n");
        else
          printk("create usbtll_disable_irq_attr successfully\n");
#endif

	ghcd = hcd = usb_create_hcd(&ehci_omap_hc_driver,
					&dev->dev, dev->dev.bus_id);
	if (!hcd)
		return -ENOMEM;

	retval = omap_start_ehc(dev, hcd);
	if (retval)
		return retval;

	hcd->rsrc_start = 0;
	hcd->rsrc_len = 0;
	hcd->rsrc_start = dev->resource[0].start;
	hcd->rsrc_len = dev->resource[0].end - dev->resource[0].start + 1;

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_err(&dev->dev, "ioremap failed\n");
		return -ENOMEM;
	}

	ehci = hcd_to_ehci(hcd);
	ehci->caps = hcd->regs;
#ifdef CONFIG_HAS_WAKELOCK
	wake_lock_init(&ehci->wake_lock_ehci_rwu,
			WAKE_LOCK_SUSPEND, "ehci_rwu");
	wake_lock_init(&ehci->wake_lock_ehci_pm,
			WAKE_LOCK_SUSPEND, "ehci_pm");
#endif

	ehci->sbrn = 0x20;

	ehci->regs = hcd->regs + HC_LENGTH(readl(&ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = readl(&ehci->caps->hcs_params);

	/* SET 1 micro-frame Interrupt interval */
	writel(readl(&ehci->regs->command) | (1<<16), &ehci->regs->command);

	retval = usb_add_hcd(hcd, dev->resource[1].start,
				IRQF_DISABLED | IRQF_SHARED);
	if (retval == 0)
		return retval;

	dev_dbg(hcd->self.controller, "ERR: add_hcd\n");
	omap_stop_ehc(dev, hcd);
	iounmap(hcd->regs);
	usb_put_hcd(hcd);

	return retval;
}

/*-------------------------------------------------------------------------*/

/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * ehci_hcd_omap_drv_remove - shutdown processing for EHCI HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_ehci_hcd_omap_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static int ehci_hcd_omap_drv_remove(struct platform_device *dev)
{
	struct usb_hcd *hcd = platform_get_drvdata(dev);
	struct ehci_omap_clock_defs *ehci_clocks;
	struct ehci_hcd *ehci;
	unsigned long flags;

	ehci = hcd_to_ehci(hcd);
	ehci_clocks = (struct ehci_omap_clock_defs *)
			(((char *)hcd_to_ehci(hcd)) + sizeof(struct ehci_hcd));

	dev_dbg(&dev->dev, "ehci_hcd_omap_drv_remove()\n");

	spin_lock_irqsave(&usbtll_clock_lock, flags);
       if (ehci_clocks->suspended) {
#if 0
		/* ICLKs are Autoidled. No need for explicit control */
		clk_enable(ehci_clocks->usbtll_ick_clk);
		clk_enable(ehci_clocks->usbhost_ick_clk);
#endif
		/* Enable All F-clks now before accessing controller */
		if (!usbtll_fclk_enabled) {
			clk_enable(ehci_clocks->usbtll_fck_clk);
			usbtll_fclk_enabled = 1;
		}
		clk_enable(ehci_clocks->usbhost1_48m_fck_clk);
		clk_enable(ehci_clocks->usbhost2_120m_fck_clk);
		ehci_clocks->suspended = 0;
	}
	spin_unlock_irqrestore(&usbtll_clock_lock, flags);

#ifdef CONFIG_HAS_WAKELOCK
	wake_lock_destroy(&ehci->wake_lock_ehci_rwu);
	wake_lock_destroy(&ehci->wake_lock_ehci_pm);
#endif

	iounmap(hcd->regs);
	usb_remove_hcd(hcd);
	omap_stop_ehc(dev, hcd);
	usb_put_hcd(hcd);

#ifdef CONFIG_MOT_FEAT_IPC_CORERETENTION
        sysfs_remove_file(power_kobj, &usbtll_disable_irq_attr.attr);
#endif

	return 0;
}

/*-------------------------------------------------------------------------*/
MODULE_ALIAS("platform:omap-ehci");
static struct platform_driver ehci_hcd_omap_driver = {
	.probe = ehci_hcd_omap_drv_probe,
	.remove = ehci_hcd_omap_drv_remove,
	.shutdown = usb_hcd_platform_shutdown,
	/*.suspend      = ehci_hcd_omap_drv_suspend, */
	/*.resume       = ehci_hcd_omap_drv_resume, */
	.driver = {
		.name = "ehci-omap",
		.bus = &platform_bus_type
	}
};
