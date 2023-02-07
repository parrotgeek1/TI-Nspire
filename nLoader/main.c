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

#include "utils.h"
#include "types.h"
#include "screen.h"
#include "imageloader.h"
#include "patchboot2.h"
#include "inject_ndless_loader.h"

asm(".section .text._start\n"
	"_start: .global _start\n"
	"ldr sp, =#0x10400000\n"
	"b main\n");

void *loadBoot2() {
	void *addr;
	void *BOOT2_PTR = *(uint32_t**)(0x111FFFF8);
	if((addr = loadTIBootImage(BOOT2_PTR)) != 0) {
		if((addr == (void *)TI_BOOT2_BASE) && patchBoot2()) {
			if(!injectNdlessLoader()) {
				puts("Injecting Ndless loader not supported for this BOOT2");
			}
		} else {
			puts("Patching not supported for this image");
		}
		return addr;
	}
	return 0;
}

void main() {
	register void *addr; /* yes, it really needs to be a register */
	// We don't need to flush icache/dcache here because nothing should ever have been executing from 0x111c0000 before us
	disableWatchdog();
	initScreen();
	puts("\r\nnLoader: Created by parrotgeek1. Version: 1.1");
#if defined(CAS_OS)
	puts("CAS OS build");
#elif defined(NONCAS_OS)
	puts("Non-CAS OS build");
#else
#error "Missing -DCAS_OS or -DNONCAS_OS in CFLAGS"
#endif
	puts("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 2, as published by the Free Software Foundation.");
	if((addr = loadBoot2()) != 0) {
		puts("nLoader: Loading complete, launching image.");
		uint32_t dummy;
		__asm volatile(
			"0: mrc p15, 0, r15, c7, c10, 3 @ test and clean DCache\n"
			"bne 0b\n"
			"mov %[dummy], #0\n"
			"mcr p15, 0, %[dummy], c7, c7, 0 @ invalidate ICache and DCache\n"
			"mov pc, %[addr]"
		: [dummy] "=r" (dummy) : [addr] "r" (addr));
	} else {
		puts("nLoader: Error reading/validating BOOT2 image");
		drawError();
	}
	while(1);
	__builtin_unreachable();
}
