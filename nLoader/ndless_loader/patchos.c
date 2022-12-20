/*
 * taken and adapted from ControlX 5.0.5 by Xavier Andréani
 * which itself is based on nLaunchy:
 *
 * Copyright (C) 2012-2013 nLaunch team
 * Copyright (C) 2013      nLaunch CX guy
 * Copyright (C) 2013-2015 Excale
 * Copyright (C) 2013-2015 Lionel Debroux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 */

#include "../types.h"
#include "patchos.h"

static inline void inline_memcpy(void *dest, void *src, size_t n) {
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}

#define NOP 0x00000000
#define PATCH_SETW(A,B) *(uint32_t *)(A) = (B)
#define PATCH_SETHW(A,B) *(uint16_t *)(A) = (B)
#define PATCH_SETB(A,B) *(uint8_t *)(A) = (B)

#define IGNORE 0

#define NOS	sizeof(os_patches)/sizeof(os_patches[0])

#define I_ID		0
#define I_FLAGS		1
#define	I_BOOT2UPD	2
#define	I_NBOOT		3
#define I_SELFD		4
#define I_ASIC1		5
#define I_ASIC2		6
#define I_SYSCALLS	7

static const uint32_t os_patches[][I_SYSCALLS+NSYSCALLS] = {
// CX/CM offsets
// OS ID			flags	Boot2upd					nBoot					self-destruct				ASIC1					ASIC2					fopen					stat					malloc					fread					fclose
{CX301_1753,		0b00,	IGNORE,						NBOOT_CX301_1753,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC301_1753,		0b01,	IGNORE,						NBOOT_CXC301_1753,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX302_179X,		0b00,	IGNORE,						NBOOT_CX302_1791,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX302_179X,		0b00,	IGNORE,						NBOOT_CX302_1793,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC302_179X,		0b01,	IGNORE,						NBOOT_CXC302_1791,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC302_179X,		0b01,	IGNORE,						NBOOT_CXC302_1793,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX310_392,			0b00,	BOOT2UPD_CX310_392,			NBOOT_CX310_392,		IGNORE,						IGNORE,					IGNORE,					FOPEN_CX310_392,		STAT_CX310_392,			MALLOC_CX310_392,		FREAD_CX310_392,		FCLOSE_CX310_392,		},
{CXC310_392,		0b01,	BOOT2UPD_CXC310_392,		NBOOT_CXC310_392,		IGNORE,						IGNORE,					IGNORE,					FOPEN_CXC310_392,		STAT_CXC310_392,		MALLOC_CXC310_392,		FREAD_CXC310_392,		FCLOSE_CXC310_392,		},
{CX320_1212,		0b00,	IGNORE,						NBOOT_CX320_121X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC320_1212,		0b01,	IGNORE,						NBOOT_CXC320_121X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX320_1219,		0b00,	IGNORE,						NBOOT_CX320_121X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC320_1219,		0b01,	IGNORE,						NBOOT_CXC320_121X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX32X_123X,		0b00,	IGNORE,						NBOOT_CX32X_123X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC323_1233,		0b01,	IGNORE,						NBOOT_CXC32X_123X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC324_1237,		0b01,	IGNORE,						NBOOT_CXC32X_123X,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX330_538,			0b00,	IGNORE,						NBOOT_CX330_538,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CXC330_538,		0b01,	IGNORE,						NBOOT_CXC330_538,		IGNORE,						IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					}, 														
{CX360_5XX,			0b00,	BOOT2UPD_CX360_5XX,			NBOOT_CX360_5XX,		SELFD_CX360_5XX,			ASIC1_CX360_5XX,		IGNORE,					FOPEN_CX360_5XX,		STAT_CX360_5XX,			MALLOC_CX360_5XX,		FREAD_CX360_5XX,		FCLOSE_CX360_5XX,		},
{CXC360_546_550,	0b01,	BOOT2UPD_CXC360_546_550,	NBOOT_CXC360_546_550,	SELFD_CXC360_546_550,		ASIC1_CXC360_546_550,	IGNORE,					FOPEN_CXC360_546_550,	STAT_CXC360_546_550,	MALLOC_CXC360_546_550,	FREAD_CXC360_546_550,	FCLOSE_CXC360_546_550,	},
{CX390_46X,			0b00,	BOOT2UPD_CX390_46X,			NBOOT_CX390_46X,		SELFD_CX390_461,			ASIC1_CX390_46X,		ASIC2_CX390_461,		IGNORE/*FOPEN_CX390_46X*/,STAT_CX390_46X,			MALLOC_CX390_46X,		FREAD_CX390_46X,		FCLOSE_CX390_46X,		},
{CX390_46X,			0b00,	BOOT2UPD_CX390_46X,			NBOOT_CX390_46X,		SELFD_CX390_463,			ASIC1_CX390_46X,		ASIC2_CX390_463,		IGNORE/*FOPEN_CX390_46X*/,STAT_CX390_46X,			MALLOC_CX390_46X,		FREAD_CX390_46X,		FCLOSE_CX390_46X,		},
{CXC390_46X,		0b01,	BOOT2UPD_CXC390_46X,		NBOOT_CXC390_46X,		SELFD_CXC390_461,			ASIC1_CXC390_46X,		ASIC2_CXC390_461,		IGNORE/*FOPEN_CXC390_46X*/,STAT_CXC390_46X,		MALLOC_CXC390_46X,		FREAD_CXC390_46X,		FCLOSE_CXC390_46X,		},
{CXC390_46X,		0b01,	BOOT2UPD_CXC390_46X,		NBOOT_CXC390_46X,		SELFD_CXC390_463,			ASIC1_CXC390_46X,		ASIC2_CXC390_463,		IGNORE/*FOPEN_CXC390_46X*/,STAT_CXC390_46X,		MALLOC_CXC390_46X,		FREAD_CXC390_46X,		FCLOSE_CXC390_46X,		},
{CX391_38,			0b00,	BOOT2UPD_CX391_38,			NBOOT_CX391_38,			SELFD_CX391_38,				ASIC1_CX391_38,			ASIC2_CX391_38,			FOPEN_CX391_38,			STAT_CX391_38,			MALLOC_CX391_38,		FREAD_CX391_38,			FCLOSE_CX391_38,		},
{CXC391_38,			0b01,	BOOT2UPD_CXC391_38,			NBOOT_CXC391_38,		SELFD_CXC391_38,			ASIC1_CXC391_38,		ASIC2_CXC391_38,		FOPEN_CXC391_38,		STAT_CXC391_38,			MALLOC_CXC391_38,		FREAD_CXC391_38,		FCLOSE_CXC391_38,		},
{CX400_235,			0b00,	BOOT2UPD_CX400_235,			NBOOT_CX400_235,		SELFD_CX400_235,			ASIC1_CX400_235,		ASIC2_CX400_235,		FOPEN_CX400_235,		STAT_CX400_235,			MALLOC_CX400_235,		FREAD_CX400_235,		FCLOSE_CX400_235,		},	
{CXC400_235,		0b01,	BOOT2UPD_CXC400_235,		NBOOT_CXC400_235,		SELFD_CXC400_235,			ASIC1_CXC400_235,		ASIC2_CXC400_235,		FOPEN_CXC400_235,		STAT_CXC400_235,		MALLOC_CXC400_235,		FREAD_CXC400_235,		FCLOSE_CXC400_235,		},
{CX402_27,			0b00,	BOOT2UPD_CX402_27,			IGNORE,					SELFD_CX402_27,				ASIC1_CX402_27,			ASIC2_CX402_27,			IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},	
{CXC402_27,			0b01,	BOOT2UPD_CXC402_27,			IGNORE,					SELFD_CXC402_27,			ASIC1_CXC402_27,		ASIC2_CXC402_27,		IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX403_29,			0b00,	BOOT2UPD_CX403_29,			IGNORE,					SELFD_CX403_29,				ASIC1_CX403_29,			ASIC2_CX403_29,			FOPEN_CX403_29,			STAT_CX403_29,			MALLOC_CX403_29,		FREAD_CX403_29,			FCLOSE_CX403_29,		},	
{CXC403_29,			0b01,	BOOT2UPD_CXC403_29,			IGNORE,					SELFD_CXC403_29,			ASIC1_CXC403_29,		ASIC2_CXC403_29,		FOPEN_CXC403_29,		STAT_CXC403_29,			MALLOC_CXC403_29,		FREAD_CXC403_29,		FCLOSE_CXC403_29,		},
{CX420_532,			0b00,	BOOT2UPD_CX420_532,			IGNORE,					SELFD_CX420_532,			ASIC1_CX420_532,		ASIC2_CX420_532,		FOPEN_CX420_532,		STAT_CX420_532,			MALLOC_CX420_532,		FREAD_CX420_532,		FCLOSE_CX420_532,		},	
{CXC420_532,		0b01,	BOOT2UPD_CXC420_532,		IGNORE,					SELFD_CXC420_532,			ASIC1_CXC420_532,		ASIC2_CXC420_532,		FOPEN_CXC420_532,		STAT_CXC420_532,		MALLOC_CXC420_532,		FREAD_CXC420_532,		FCLOSE_CXC420_532,		},
{CXC430_547,		0b01,	BOOT2UPD_CXC430_547,		IGNORE,					SELFD_CXC430_547,			ASIC1_CXC430_547,		ASIC2_CXC430_547,		FOPEN_CXC430_547,		STAT_CXC430_547,		MALLOC_CXC430_547,		FREAD_CXC430_547,		FCLOSE_CXC430_547,		},
{CX430_702,			0b00,	BOOT2UPD_CX430_702,			IGNORE,					SELFD_CX430_702,			ASIC1_CX430_702,		ASIC2_CX430_702,		FOPEN_CX430_702,		STAT_CX430_702,			MALLOC_CX430_702,		FREAD_CX430_702,		FCLOSE_CX430_702,		},
{CXC430_702,		0b01,	BOOT2UPD_CXC430_702,		IGNORE,					SELFD_CXC430_702,			ASIC1_CXC430_702,		ASIC2_CXC430_702,		FOPEN_CXC430_702,		STAT_CXC430_702,		MALLOC_CXC430_702,		FREAD_CXC430_702,		FCLOSE_CXC430_702,		},
{CXC440_531,		0b01,	BOOT2UPD_CXC440_53X,		IGNORE,					SELFD_CXC440_531,			ASIC1_CXC440_53X,		ASIC2_CXC440_531,		FOPEN_CXC440_531,		STAT_CXC440_531,		MALLOC_CXC440_531,		FREAD_CXC440_531,		FCLOSE_CXC440_531,		},
{CX440_532,			0b00,	BOOT2UPD_CX440_532,			IGNORE,					SELFD_CX440_532,			ASIC1_CX440_532,		ASIC2_CX440_532,		FOPEN_CX440_532,		STAT_CX440_532,			MALLOC_CX440_532,		FREAD_CX440_532,		FCLOSE_CX440_532,		},
{CXC440_532,		0b01,	BOOT2UPD_CXC440_53X,		IGNORE,					SELFD_CXC440_532,			ASIC1_CXC440_53X,		ASIC2_CXC440_532,		FOPEN_CXC440_532,		STAT_CXC440_532,		MALLOC_CXC440_532,		FREAD_CXC440_532,		FCLOSE_CXC440_532,		},
{CX450_845,			0b00,	BOOT2UPD_CX450_845,			IGNORE,					SELFD_CX450_845,			ASIC1_CX450_845,		ASIC2_CX450_845,		IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},
{CX450_1180,		0b00,	BOOT2UPD_CX450_1180,		IGNORE,					SELFD_CX450_1180,			ASIC1_CX450_1180,		ASIC2_CX450_1180,		FOPEN_CX450_1180,		STAT_CX450_1180,		MALLOC_CX450_1180,		FREAD_CX450_1180,		FCLOSE_CX450_1180,		},
{CXC450_1180,		0b01,	BOOT2UPD_CXC450_1180,		IGNORE,					SELFD_CXC450_1180,			ASIC1_CXC450_1180,		ASIC2_CXC450_1180,		FOPEN_CXC450_1180,		STAT_CXC450_1180,		MALLOC_CXC450_1180,		FREAD_CXC450_1180,		FCLOSE_CXC450_1180,		},
{CX451_X,			0b00,	BOOT2UPD_CX451_X,			IGNORE,					SELFD_CX451_X,				ASIC1_CX451_X,			ASIC2_CX451_X,			FOPEN_CX451_12,			STAT_CX451_12,			MALLOC_CX451_12,		FREAD_CX451_12,			FCLOSE_CX451_12,		},
{CXC451_X,			0b01,	BOOT2UPD_CXC451_X,			IGNORE,					SELFD_CXC451_X,				ASIC1_CXC451_X,			ASIC2_CXC451_X,			FOPEN_CXC451_12,		STAT_CXC451_12,			MALLOC_CXC451_12,		FREAD_CXC451_12,		FCLOSE_CXC451_12,		},
{CX452_8,			0b00,	BOOT2UPD_CX4523_X,			IGNORE,					SELFD_CX4523_X,				ASIC1_CX4523_X,			ASIC2_CX452_8,			IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},	
{CXC452_8,			0b01,	BOOT2UPD_CXC4523_X,			IGNORE,					SELFD_CXC4523_X,			ASIC1_CXC4523_X,		ASIC2_CXC452_8,			IGNORE,					IGNORE,					IGNORE,					IGNORE,					IGNORE,					},	
{CX453_14,			0b00,	BOOT2UPD_CX4523_X,			IGNORE,					SELFD_CX4523_X,				ASIC1_CX4523_X,			ASIC2_CX453_14,			FOPEN_CX453_14,			STAT_CX453_14,			MALLOC_CX453_14,		FREAD_CX453_14,			FCLOSE_CX453_14,		},	
{CXC453_14,			0b01,	BOOT2UPD_CXC4523_X,			IGNORE,					SELFD_CXC4523_X,			ASIC1_CXC4523_X,		ASIC2_CXC453_14,		FOPEN_CXC453_14,		STAT_CXC453_14,			MALLOC_CXC453_14,		FREAD_CXC453_14,		FCLOSE_CXC453_14,		},	
{CX455_79,			0b00,	BOOT2UPD_CX455_79,			IGNORE,					SELFD_CX455_79,				ASIC1_CX455_79,			ASIC2_CX455_79,			FOPEN_CX455_79,			STAT_CX455_79,			MALLOC_CX455_79,		FREAD_CX455_79,			FCLOSE_CX455_79,					},	
{CX454_48,			0b00,	BOOT2UPD_CX454_48,			IGNORE,					SELFD_CX454_48,				ASIC1_CX454_48,			ASIC2_CX454_48,			FOPEN_CX454_48,			STAT_CX454_48,			MALLOC_CX454_48,		FREAD_CX454_48,			FCLOSE_CX454_48,					},
{CXC454_48,			0b01,	BOOT2UPD_CXC454_48,			IGNORE,					SELFD_CXC454_48,			ASIC1_CXC454_48,		ASIC2_CXC454_48,		FOPEN_CXC454_48,		STAT_CXC454_48,			MALLOC_CXC454_48,		FREAD_CXC454_48,		FCLOSE_CXC454_48,					},
{CXC455_79,			0b01,	BOOT2UPD_CXC455_79,			IGNORE,					SELFD_CXC455_79,			ASIC1_CXC455_79,		ASIC2_CXC455_79,		FOPEN_CXC455_79,		STAT_CXC455_79,			MALLOC_CXC455_79,		FREAD_CXC455_79,		FCLOSE_CXC455_79,					},	
};

static inline uint8_t getOSIndex(uint32_t os_id, uint32_t base) {
	uint8_t i=0;
	for(i=0;i<NOS;i++) {
		if(os_patches[i][I_ID]==os_id)
			break;
	}
	// check "/phoenix/install/TI-Nspire.tnc" string location if necessary
	if(	(os_id==CX302_179X && *((volatile uint32_t *)(base+0x007D60D0)) != 0x6F68702F) ||
		(os_id==CXC302_179X && *((volatile uint32_t *)(base+0x00831D08)) != 0x6F68702F) ||
		(os_id==CX390_46X && *((volatile uint32_t *)(base+0x009A9584)) != 0x6F68702F) ||
		(os_id==CXC390_46X && *((volatile uint32_t *)(base+0x00A0A004)) != 0x6F68702F)
	)
		i++;
	return i;
}

//! Patch the OS.
static inline void patch_OS(uint32_t base) {
	static uint32_t ndless_loader[] = { NDLESS_LOADER };
	unsigned char asicflags = ((*(volatile unsigned int*) 0x900A002C)>>26)&0b11111; /* can't be a function here */
	uint32_t os_id = *((uint32_t*)(base+0x20));
	uint8_t os_ind = getOSIndex(os_id,base);
	if(os_ind<NOS) {
		if(os_patches[os_ind][I_BOOT2UPD]) {
			// if ndless is supported on this OS, replace the boot2 updater with the ndless loader
			if(os_patches[os_ind][I_SYSCALLS]) {
				inline_memcpy((void *)(ndless_loader+OFFSET_MODEL), (void *)&(os_patches[os_ind][I_SYSCALLS]), NSYSCALLS*sizeof(uint32_t));
				inline_memcpy((void *)(base+(os_patches[os_ind][I_BOOT2UPD]&~0x10000000)),ndless_loader,sizeof(ndless_loader));
			}
			else
				// otherwise nuke the boot2 updater (replace with bx lr - a return statement)
				PATCH_SETW(base+(os_patches[os_ind][I_BOOT2UPD]&~0x10000000),0xE12FFF1E);
		}
		// you shouldn't be using nBoot and nLoader together, but this avoids OS < 4.0.1 crashing if you do
		if(os_patches[os_ind][I_NBOOT])	PATCH_SETW(base+(os_patches[os_ind][I_NBOOT]&~0x10000000),NOP);
		// disable integrity check which deletes the OS and reboots if it's modified, allowing nTNOC to work again
		if(os_patches[os_ind][I_SELFD])	PATCH_SETW(base+(os_patches[os_ind][I_SELFD]&~0x10000000),0xE12FFF1E);
		// skip asic patch if flags match
		if(os_patches[os_ind][I_ASIC1] && asicflags != os_patches[os_ind][I_FLAGS]) {
			PATCH_SETB(base+(os_patches[os_ind][I_ASIC1]&~0x10000000),0x54);
			PATCH_SETW(base+((os_patches[os_ind][I_ASIC1]&~0x10000000)+0x17),os_patches[os_ind][I_FLAGS]?0xE3A04341:0xE3A04000); // CX CAS/num ASIC2
			PATCH_SETW(base+((os_patches[os_ind][I_ASIC1]&~0x10000000)+0x1B),0);
			PATCH_SETW(base+((os_patches[os_ind][I_ASIC1]&~0x10000000)+0x48B),os_patches[os_ind][I_FLAGS]?0x00010105:0); // CX CAS/num ASIC1
		}
		if(os_patches[os_ind][I_ASIC2] && asicflags != os_patches[os_ind][I_FLAGS]) {
			PATCH_SETHW(base+(os_patches[os_ind][I_ASIC2]&~0x10000000),0x2110);
			PATCH_SETW(base+((os_patches[os_ind][I_ASIC2]&~0x10000000)+8),0);
			PATCH_SETW(base+((os_patches[os_ind][I_ASIC2]&~0x10000000)+0x118),os_patches[os_ind][I_FLAGS]?0x04000001:0); // CX CAS/num ASIC2
		}
	}
}
