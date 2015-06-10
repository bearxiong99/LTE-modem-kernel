/*
 * arch/arm/plat-omap/include/mach/io.h
 *
 * IO definitions for TI OMAP processors and boards
 *
 * Copied from arch/arm/mach-sa1100/include/mach/io.h
 * Copyright (C) 1997-1999 Russell King
 * Copyright (C) 2009 Motorola
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Revision History:
 *  06-12-1997	RMK	Created.
 *  07-04-1999	RMK	Major cleanup
 *  24-Aug-2009 Motorola  Support for W3G architecture
 */

#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#include <mach/hardware.h>

#define IO_SPACE_LIMIT 0xffffffff

/*
 * We don't actually have real ISA nor PCI buses, but there is so many
 * drivers out there that might just work if we fake them...
 */
#define __io(a)		__typesafe_io(a)
#define __mem_pci(a)	(a)

/*
 * ----------------------------------------------------------------------------
 * I/O mapping
 * ----------------------------------------------------------------------------
 */

#if defined(CONFIG_ARCH_OMAP1)

#define IO_PHYS			0xFFFB0000
#define IO_OFFSET		0x01000000	/* Virtual IO = 0xfefb0000 */
#define IO_SIZE			0x40000
#define IO_VIRT			(IO_PHYS - IO_OFFSET)
#define __IO_ADDRESS(pa)	((pa) - IO_OFFSET)
#define __OMAP1_IO_ADDRESS(pa)	((pa) - IO_OFFSET)
#define io_v2p(va)		((va) + IO_OFFSET)

#elif defined(CONFIG_ARCH_OMAPW3G)

#define L3_W3G_PHYS             L3_W3G_BASE  /* 0x46000000 */
#define L3_W3G_VIRT             0xd8000000
#define L3_W3G_SIZE             SZ_1M

#define L3_W3G_INC_PHYS         0x46100000 /* Interrupt controller base*/ 
#define L3_W3G_INC_VIRT         0xd8100000
#define L3_W3G_INC_SIZE         SZ_2M

#ifdef CONFIG_MACH_W3G_LTE_DATACARD
#define W3G_IPC_SHARED_MEMORY_PHYS 0x80800000           /*AP/BP shared memory*/ 
#define W3G_IPC_SHARED_MEMORY_VIRT 0xD8300000
#define W3G_IPC_SHARED_MEMORY_SIZE  SZ_1M 
#else
#define W3G_IPC_SHARED_MEMORY_PHYS 0x80700000           /*AP/BP shared memory*/ 
#define W3G_IPC_SHARED_MEMORY_VIRT 0xD8300000
#define W3G_IPC_SHARED_MEMORY_SIZE  SZ_1M 
#endif

#ifdef CONFIG_MACH_W3G_LTE_DATACARD
#define W3G_4GHIF_IPC_SHARED_MEMORY_PHYS 0x81000000     /*AP/BP 4GHIF shared memory*/
#define W3G_4GHIF_IPC_SHARED_MEMORY_VIRT 0xD8400000
#define W3G_4GHIF_IPC_SHARED_MEMORY_SIZE  SZ_1M
#endif

#define L4_W3G_APPS_PHYS        0x47000000 /* L3-L4 Apps control */
#define L4_W3G_APPS_VIRT        0xd9000000
#define L4_W3G_APPS_SIZE        SZ_1M

#define L3_MDMDSP_PHYS          0x5c000000 /* Base of DSP memories*/
#define L3_MDMDSP_VIRT          0xee000000
#define L3_MDMDSP_SIZE          SZ_1M

#define L3_L4_PER_PHYS          0x5C100000 /* M2SRAM ,SDRC,GPMC and L4-2G peripherals*/
#define L3_L4_PER_VIRT          0xee100000  
#define L3_L4_PER_SIZE          SZ_1M

#define L4_3G_BASE_PHYS         0x5C200000 /* peripherals on L4 -3G*/
#define L4_3G_BASE_VIRT         0xee200000
#define L4_3G_BASE_SIZE         SZ_1M

#define L4_EMU_BASE_PHYS        0x5C300000  /* peripherals on L4-EMU */
#define L4_EMU_BASE_VIRT        0xee300000
#define L4_EMU_BASE_SIZE        SZ_2M

#define IO_OFFSET		0x92000000
#define __IO_ADDRESS(pa)	((pa) + IO_OFFSET)	/* Works for L3 and L4 */
#define __OMAP2_IO_ADDRESS(pa)	((pa) + IO_OFFSET)	/* Works for L3 and L4 */
#define io_v2p(va)		((va) - IO_OFFSET)	/* Works for L3 and L4 */
#define io_p2v(pa)              __IO_ADDRESS(pa)

#elif defined(CONFIG_ARCH_OMAP2) 

/* We map both L3 and L4 on OMAP2 */
#define L3_24XX_PHYS	L3_24XX_BASE	/* 0x68000000 */
#define L3_24XX_VIRT	0xf8000000
#define L3_24XX_SIZE	SZ_1M		/* 44kB of 128MB used, want 1MB sect */
#define L4_24XX_PHYS	L4_24XX_BASE	/* 0x48000000 */
#define L4_24XX_VIRT	0xd8000000
#define L4_24XX_SIZE	SZ_1M		/* 1MB of 128MB used, want 1MB sect */

#define L4_WK_243X_PHYS		L4_WK_243X_BASE		/* 0x49000000 */
#define L4_WK_243X_VIRT		0xd9000000
#define L4_WK_243X_SIZE		SZ_1M
#define OMAP243X_GPMC_PHYS	OMAP243X_GPMC_BASE	/* 0x49000000 */
#define OMAP243X_GPMC_VIRT	0xFE000000
#define OMAP243X_GPMC_SIZE	SZ_1M
#define OMAP243X_SDRC_PHYS	OMAP243X_SDRC_BASE
#define OMAP243X_SDRC_VIRT	0xFD000000
#define OMAP243X_SDRC_SIZE	SZ_1M
#define OMAP243X_SMS_PHYS	OMAP243X_SMS_BASE
#define OMAP243X_SMS_VIRT	0xFC000000
#define OMAP243X_SMS_SIZE	SZ_1M

#ifndef CONFIG_ARCH_OMAPW3G
#define IO_OFFSET		0x90000000
#define __IO_ADDRESS(pa)	((pa) + IO_OFFSET) /* Works for L3 and L4 */
#define __OMAP2_IO_ADDRESS(pa)	((pa) + IO_OFFSET) /* Works for L3 and L4 */
#define io_v2p(va)		((va) - IO_OFFSET) /* Works for L3 and L4 */
#define io_p2v(pa)		((pa) + IO_OFFSET) /* Works for L3 and L4 */
#endif

/* DSP */
#define DSP_MEM_24XX_PHYS	OMAP2420_DSP_MEM_BASE	/* 0x58000000 */
#define DSP_MEM_24XX_VIRT	0xe0000000
#define DSP_MEM_24XX_SIZE	0x28000
#define DSP_IPI_24XX_PHYS	OMAP2420_DSP_IPI_BASE	/* 0x59000000 */
#define DSP_IPI_24XX_VIRT	0xe1000000
#define DSP_IPI_24XX_SIZE	SZ_4K
#define DSP_MMU_24XX_PHYS	OMAP2420_DSP_MMU_BASE	/* 0x5a000000 */
#define DSP_MMU_24XX_VIRT	0xe2000000
#define DSP_MMU_24XX_SIZE	SZ_4K

#ifdef CONFIG_ARCH_OMAPW3G /* For Wrigley */
#define L3_W3G_PHYS         L3_W3G_BASE  /* 0x46000000 */
#define L3_W3G_VIRT         0xd8000000
#define L3_W3G_SIZE         SZ_1M

#define L3_W3G_INC_PHYS     0x46100000 /* Interrupt controller base */
#define L3_W3G_INC_VIRT     0xd8100000
#define L3_W3G_INC_SIZE     SZ_2M

#ifdef CONFIG_MACH_W3G_LTE_DATACARD
#define W3G_IPC_SHARED_MEMORY_PHYS         0x80800000 /* AP/BP shared memory */
#define W3G_IPC_SHARED_MEMORY_VIRT         0xd8300000
#define W3G_IPC_SHARED_MEMORYSIZE          SZ_1M
#else
#define W3G_IPC_SHARED_MEMORY_PHYS         0x80700000 /* AP/BP shared memory */
#define W3G_IPC_SHARED_MEMORY_VIRT         0xd8300000
#define W3G_IPC_SHARED_MEMORYSIZE          SZ_1M
#endif

#define L4_W3G_APPS_PHYS         0x47000000 /* L3-L4 Apps controller */
#define L4_W3G_APPS_VIRT         0xd9000000
#define L4_W3G_APPS_SIZE         SZ_1M

#define L3_MDMDSP_PHYS           0x5c000000 /* Base of DSP memories */
#define L3_MDMDSP_VIRT           0xee000000
#define L3_MDMDSP_SIZE           SZ_1M

#define L3_L4_PER_PHYS           0x5c100000 /* M2SRAM, SDRC, GPMC, and L4-2G peripherals */
#define L3_L4_PER_VIRT           0xee100000
#define L3_L4_PER_SIZE           SZ_1M

#define L4_3G_BASE_PHYS          0x5C200000 /* peripherals on L4 -3G */
#define L4_3G_BASE_VIRT          0xee200000
#define L4_3G_BASE_SIZE          SZ_1M

#define L4_EMU_BASE_PHYS         0x5C300000  /* peripherals on L4-EMU */
#define L4_EMU_BASE_VIRT         0xee300000
#define L4_EMU_BASE_SIZE         SZ_2M

#define IO_OFFSET		0x92000000
#define __IO_ADDRESS(pa)	((pa) + IO_OFFSET)	/* Works for L3 and L4 */
#define __OMAP2_IO_ADDRESS(pa)	((pa) + IO_OFFSET)	/* Works for L3 and L4 */
#define io_v2p(va)		((va) - IO_OFFSET)	/* Works for L3 and L4 */
#define io_p2v(pa)  __IO_ADDRESS(pa)

#endif

#elif defined(CONFIG_ARCH_OMAP3)

/* We map both L3 and L4 on OMAP3 */
#define L3_34XX_PHYS		L3_34XX_BASE	/* 0x68000000 */
#define L3_34XX_VIRT		0xf8000000
#define L3_34XX_SIZE		SZ_1M   /* 44kB of 128MB used, want 1MB sect */

#define L4_34XX_PHYS		L4_34XX_BASE	/* 0x48000000 */
#define L4_34XX_VIRT		0xd8000000
#define L4_34XX_SIZE		SZ_4M   /* 1MB of 128MB used, want 1MB sect */

/*
 * Need to look at the Size 4M for L4.
 * VPOM3430 was not working for Int controller
 */

#define L4_WK_34XX_PHYS		L4_WK_34XX_BASE /* 0x48300000 */
#define L4_WK_34XX_VIRT		0xd8300000
#define L4_WK_34XX_SIZE		SZ_1M

#define L4_PER_34XX_PHYS	L4_PER_34XX_BASE /* 0x49000000 */
#define L4_PER_34XX_VIRT	0xd9000000
#define L4_PER_34XX_SIZE	SZ_1M

#define L4_EMU_34XX_PHYS	L4_EMU_34XX_BASE /* 0x54000000 */
#define L4_EMU_34XX_VIRT	0xe4000000
#define L4_EMU_34XX_SIZE	SZ_64M

#define OMAP34XX_GPMC_PHYS	OMAP34XX_GPMC_BASE /* 0x6E000000 */
#define OMAP34XX_GPMC_VIRT	0xFE000000
#define OMAP34XX_GPMC_SIZE	SZ_1M

#define OMAP343X_SMS_PHYS	OMAP343X_SMS_BASE /* 0x6C000000 */
#define OMAP343X_SMS_VIRT	0xFC000000
#define OMAP343X_SMS_SIZE	SZ_1M

#define OMAP343X_SDRC_PHYS	OMAP343X_SDRC_BASE /* 0x6D000000 */
#define OMAP343X_SDRC_VIRT	0xFD000000
#define OMAP343X_SDRC_SIZE	SZ_1M


#define IO_OFFSET		0x90000000
#define __IO_ADDRESS(pa)	((pa) + IO_OFFSET) /* Works for L3 and L4 */
#define __OMAP2_IO_ADDRESS(pa)	((pa) + IO_OFFSET) /* Works for L3 and L4 */
#define io_v2p(va)		((va) - IO_OFFSET) /* Works for L3 and L4 */
#define io_p2v(pa)		((pa) + IO_OFFSET) /* Works for L3 and L4 */

/* DSP */
#define DSP_MEM_34XX_PHYS	OMAP34XX_DSP_MEM_BASE	/* 0x58000000 */
#define DSP_MEM_34XX_VIRT	0xe0000000
#define DSP_MEM_34XX_SIZE	0x28000
#define DSP_IPI_34XX_PHYS	OMAP34XX_DSP_IPI_BASE	/* 0x59000000 */
#define DSP_IPI_34XX_VIRT	0xe1000000
#define DSP_IPI_34XX_SIZE	SZ_4K
#define DSP_MMU_34XX_PHYS	OMAP34XX_DSP_MMU_BASE	/* 0x5a000000 */
#define DSP_MMU_34XX_VIRT	0xe2000000
#define DSP_MMU_34XX_SIZE	SZ_4K

#endif

#define IO_ADDRESS(pa)		IOMEM(__IO_ADDRESS(pa))
#define OMAP1_IO_ADDRESS(pa)	IOMEM(__OMAP1_IO_ADDRESS(pa))
#define OMAP2_IO_ADDRESS(pa)	IOMEM(__OMAP2_IO_ADDRESS(pa))

#ifdef __ASSEMBLER__
#define IOMEM(x)		x
#else
#define IOMEM(x)		((void __force __iomem *)(x))

/*
 * Functions to access the OMAP IO region
 *
 * NOTE: - Use omap_read/write[bwl] for physical register addresses
 *	 - Use __raw_read/write[bwl]() for virtual register addresses
 *	 - Use IO_ADDRESS(phys_addr) to convert registers to virtual addresses
 *	 - DO NOT use hardcoded virtual addresses to allow changing the
 *	   IO address space again if needed
 */
#define omap_readb(a)		__raw_readb(IO_ADDRESS(a))
#define omap_readw(a)		__raw_readw(IO_ADDRESS(a))
#define omap_readl(a)		__raw_readl(IO_ADDRESS(a))

#define omap_writeb(v,a)	__raw_writeb(v, IO_ADDRESS(a))
#define omap_writew(v,a)	__raw_writew(v, IO_ADDRESS(a))
#define omap_writel(v,a)	__raw_writel(v, IO_ADDRESS(a))

struct omap_sdrc_params;
struct omap_opp;

extern void omap1_map_common_io(void);
extern void omap1_init_common_hw(void);

extern void omap2_map_common_io(void);
extern void omap2_init_common_hw(struct omap_sdrc_params *sp,
				 struct omap_opp *mpu_opps,
				 struct omap_opp *dsp_opps,
				 struct omap_opp *l3_opps);

#define __arch_ioremap(p,s,t)	omap_ioremap(p,s,t)
#define __arch_iounmap(v)	omap_iounmap(v)

void __iomem *omap_ioremap(unsigned long phys, size_t size, unsigned int type);
void omap_iounmap(volatile void __iomem *addr);

#endif

#endif
