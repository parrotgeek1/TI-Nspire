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

#include "../types.h"
#include "patchos.c"

#define pBootedBoot2Ver 0xA4012EB4

void __attribute__((naked)) ndless_loader(void) {
	uint32_t id=*((volatile uint32_t *)0x10000020);

	// put back the matching boot2 version for the OS
	// the running boot2 writes its version at 0xA4012EB4 and the OS uses this instead of reading the version from flash
	*(volatile uint32_t*)pBootedBoot2Ver = 0xFFFFFFFF; // if unknown OS protect against erasing nLoader by faking highest possible boot2 version
	if(id == CXC301_1753 || id == CX301_1753) // 3.0.1.131
		*(volatile uint32_t*)pBootedBoot2Ver = 0x03010083;
	else if(id < CX310_392) // 3.0.2.141
		*(volatile uint32_t*)pBootedBoot2Ver = 0x0302008D;
	else if(id < CXC324_1237 || id == CXC323_1233 || (id == CX32X_123X && (*((uint32_t *)0x108DF348) == 0x37333231))) // 3.1.0.16
		*(volatile uint32_t*)pBootedBoot2Ver = 0x030A0010;
	else if(id < CX391_38) // 3.2.4.7
		*(volatile uint32_t*)pBootedBoot2Ver = 0x03180007;
	else if(id < CX400_235) // 3.9.1.34
		*(volatile uint32_t*)pBootedBoot2Ver = 0x035B0022;
	else if(id < CXC403_29 && id != CX403_29) // 4.0.2.47
		*(volatile uint32_t*)pBootedBoot2Ver = 0x0402002F;
	else if(id < CX430_702) // 4.0.3.49
		*(volatile uint32_t*)pBootedBoot2Ver = 0x04030031;
	else if(id == CXC430_547) // 4.3.0.3
		*(volatile uint32_t*)pBootedBoot2Ver = 0x041E0003;
	else if(id < CXC440_532) // 4.3.0.5
		*(volatile uint32_t*)pBootedBoot2Ver = 0x041E0005;
	else if(id < CX451_X && id != CX455_79 && id != CXC455_79) // 4.4.0.8
		*(volatile uint32_t*)pBootedBoot2Ver = 0x04280008;
	else if(id <= CXC454_48) // 4.5.0.14
		*(volatile uint32_t*)pBootedBoot2Ver = 0x0432000E;

	patch_OS();

	// jump back to after it prints launching image
	// this address is the same for both boot2 4.4 and 4.5
	// we can also get here from 1187d334, but the original return point (1187d33c) immediately jumps to 1187d308, so that's fine
	__asm volatile("ldr pc, =0x1187d308");
	__builtin_unreachable();
}
