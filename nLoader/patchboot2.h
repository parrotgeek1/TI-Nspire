/*
 * taken and adapted from ControlX 5.0.5 by Xavier Andréani
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

#ifndef _PATCHBOOT2_H
#define _PATCHBOOT2_H

int patchBoot2();

#define TI_BOOT2_BASE	0x11800000
// it's really the boot2 data abort handler address - just a way to measure uniqueness 
#define TI_BOOT2_ID_ADDR	(TI_BOOT2_BASE+0x20)

// the running boot2 writes its version at 0xA4012EB4
// the OS uses this instead of reading the version from flash
// this luckily allows us to spoof the version
#define BOOTED_BOOT2_VER_LOC	0xA4012EB4

#define CXB300_0	0x118205D0
#define CXB310_16	0x1189B110
#define CXB391_34	0x1189B840
#define CMB310_0	0x1189B9B0
#define CXB301_131	0x1189B9D0
#define CMB310_16	0x1189BA20
#define CXB302_141	0x1189BAA0
#define CXB324_7	0x1189BDD0
#define CXB402_47	0x1189BD70
#define CXB403_49	0x1189BE30
#define CXB430_X	0x1189BE80
#define CXB440_8	0x1189BEA0
#define CXB450_14	0x1189BEC0

// asic: 2C 00 8D E5 01 00 A0 E3 28 50 93 E5 2C 40 93 E5
// mask: FF 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF
// 1st 0x34 a little before this
#define ASIC_CXB310_16		0x118B7CF9 // bad
#define ASIC_CXB301_131		0x118B7D9D
#define ASIC_CXB302_141		0x118B7EED
#define ASIC_CXB402_47		0x118B8855
#define ASIC_CXB403_49		0x118B8915
#define ASIC_CXB450_14		0x118B89B5
#define ASIC_CXB430_5		0x118B89FD
#define ASIC_CXB430_3		0x118B8A05
#define ASIC_CXB440_8		0x118B8A1D
#define ASIC_CXB324_7		0x118B8CC9
#define ASIC_CXB391_34		0x118B8F09
#define ASIC_CXB300_0		0x118BBC91

// manuf: B1 00 95 E1 60
#define MANUF_CMB310_0		0x118b87dc
#define MANUF_CMB310_16		0x118b88cc
#define MANUF_CXB310_16		0x118b8998
#define MANUF_CXB301_131	0x118b8a90
#define MANUF_CXB302_141	0x118b8be0
#define MANUF_CXB402_47		0x118b90ac
#define MANUF_CXB403_49		0x118b916c
#define MANUF_CXB450_14		0x118b920c
#define MANUF_CXB430_5		0x118b9254
#define MANUF_CXB430_3		0x118b925c
#define MANUF_CXB440_8		0x118b9274
#define MANUF_CXB324_7		0x118b9940
#define MANUF_CXB391_34		0x118b9b0c
#define MANUF_CXB300_0		0x118bc93c

/*
 OS downgrade check patch
 The first cmp to 0x928CC6AA (Bootdata magic number at the start of each flash page)
 Make it never true so that Boot2 never finds the bootdata and therefore never sees the min OS version with in
 
 RAM:118BE690 loc_118BE690                            ; CODE XREF: sub_118BE600+6C↑j
 RAM:118BE690                                         ; sub_118BE600+B4↓j ...
 RAM:118BE690                 LDR             R0, [R4]
 RAM:118BE694                 LDR             LR, =0x928CC6AA
 RAM:118BE698                 CMP             R0, LR
 RAM:118BE69C                 BNE             loc_118BE644
 */

#define DOWNGRADE_CXB440_8	0x118BE690
#define DOWNGRADE_CXB450_14	0x118BE628

#endif
