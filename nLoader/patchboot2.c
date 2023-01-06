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

#include "types.h"
#include "patchboot2.h"
#include "utils.h"

#define IGNORE 0
#define NOP 0xE1A00000
#define PATCH_SETW(A,B) *(uint32_t *)(A) = (B)
#define PATCH_SETB(A,B) *(uint8_t *)(A) = (B)

#define IB2_ID		0
#define IB2_ASIC	1
#define IB2_MANUF	2
#define IB2_DOWNGRADE	3

#define NBOOT2 sizeof(boot2_patches)/sizeof(boot2_patches[0])
static const uint32_t boot2_patches[][IB2_DOWNGRADE+1] = {
//	Boot2 ID			ASIC			Manuf modelID			Downgrade check
	{CXB440_8,			ASIC_CXB440_8,		MANUF_CXB440_8,			DOWNGRADE_CXB440_8},
	{CXB450_14,			ASIC_CXB450_14,		MANUF_CXB450_14,		DOWNGRADE_CXB450_14},
};

uint8_t getBoot2Index(uint32_t id) {
	uint8_t i=0;
	for(i=0;i<NBOOT2;i++) {
		if(boot2_patches[i][IB2_ID]==id)
			break;
	}
	return i;
}

//! Patch the Boot2.

#ifndef CAS_OS
// CX
#define DESIRED_ASIC 0
#define FAKEASIC1 0
#define FAKEASIC2 0xE3A04000
#define MODELID 0x10
#else
// CX CAS
#define DESIRED_ASIC 1
#define FAKEASIC1 0x00010105
#define FAKEASIC2 0xE3A04341
#define MODELID 0x0F
#endif

int patch_Boot2() {
	uint32_t id = *((uint32_t*)(0x11800020));
	uint8_t index = getBoot2Index(id);
	unsigned char asicflags = ((*(volatile unsigned int*) 0x900A002C)>>26)&0b11111; // see Hackspire "Memory-mapped I/O ports on CX"

    if(index<NBOOT2){
		// see patchboot2.h for info
        if(asicflags != DESIRED_ASIC) {
            if(boot2_patches[index][IB2_MANUF]) {
                PATCH_SETW(boot2_patches[index][IB2_MANUF],0xE3A00000|MODELID);
            }
            if(boot2_patches[index][IB2_ASIC]) {
                PATCH_SETB(boot2_patches[index][IB2_ASIC],0x54);
                PATCH_SETW(boot2_patches[index][IB2_ASIC]+0x17,FAKEASIC2);
                PATCH_SETW(boot2_patches[index][IB2_ASIC]+0x1B,NOP);
                PATCH_SETW(boot2_patches[index][IB2_ASIC]+0x48B,FAKEASIC1);
            }
        }
        if(boot2_patches[index][IB2_DOWNGRADE]) {
            PATCH_SETW(boot2_patches[index][IB2_DOWNGRADE],0xE3A00000); // so comparison will never be true
        }
        return 1;
    }
    return 0;
}
