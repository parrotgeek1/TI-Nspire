/*
 * nLoader
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
#include "inject_ndless_loader.h"

asm(R"nLoader(
	.global __ndlessLoaderStart__
	.global __ndlessLoaderEnd__
	__ndlessLoaderStart__:
	.incbin "ndless_loader.bin"
	__ndlessLoaderEnd__:
	)nLoader"
);

int injectNdlessLoader() {
	uint32_t ID = *((uint32_t*)TI_BOOT2_ID_ADDR);
	if(ID == CXB440_8 || ID == CXB450_14) {
		void *ndlessLoaderStart=&__ndlessLoaderStart__, *ndlessLoaderEnd=&__ndlessLoaderEnd__;
		memcpy((void *)NDLESS_LOC, ndlessLoaderStart, (unsigned)(ndlessLoaderEnd-ndlessLoaderStart));
		// hijack where it prints launching image
		// somehow this is the same in 4.0.3 and 4.4.0.8 and 4.5.0.14. lucky!
		/*
		 RAM:118EC8B0 aBoot2Loading_0 DCB 0xD,0xA             ; DATA XREF: sub_1187D2DC:loc_1187D334↑o
		 RAM:118EC8B0                                         ; RAM:off_1187D348↑o
		 RAM:118EC8B0                 DCB "BOOT2: loading complete (%d ticks), launching image.",0xD,0xA,0

		 RAM:1187D2FC                 ADD     R2, R4, #0x6C
		 RAM:1187D300                 LDR     R0, =aBoot2LoadingCo ; "\r\nBOOT2: loading complete (%d ticks),"...
		 RAM:1187D304                 BL      sub_118D08E4
		*/
		*(volatile uint32_t *)0x1187d300 = 0xe51ff004; // jump to below value
		*(volatile uint32_t *)0x1187d304 = NDLESS_LOC;
		// the other one without <%s>
		*(volatile uint32_t *)0x1187d334 = 0xe51ff004; // jump to below value
		*(volatile uint32_t *)0x1187d338 = NDLESS_LOC;
		return 1;
	} else {
		// This boot2 isn't supported
		return 0;
	}
}
