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
#include "screen.h"
#include "imageloader.h"
#include "patchboot2.h"
#include "inject_ndless_loader.h"
#include "casos.h"

asm(".section .text._start\n"
    "_start: .global _start\n"
    "ldr sp, =#0x10400000\n"
    "b main\n");

int load_boot2() {
    char *BOOT2_PTR = *(char**)(0x111FFFF8);
    if(decompressFiles(BOOT2_PTR,(void *) 0x11800000)) {
        if(!patch_Boot2()) {
            puts("Can't patch this boot2, but continuing anyway to avoid bricks");
            return 1;
        } else {
            if(!inject_ndless_loader()) {
                puts("Injecting Ndless loader not supported by this boot2");
                return 1;
            }
        }
        return 1;
    } else {
        puts("Can't decompress boot2");
        return 0;
    }
}

void main() {
    // We don't need to flush icache/dcache here because nothing should ever have been executing from 111c0000 before us
    ut_disable_watchdog();
    init_screen();
    puts("\r\nnLoader: Created by parrotgeek1. Version: 1.1");
#ifdef CAS_OS
        puts("CAS OS build");
#else
        puts("Non-CAS OS build");
#endif
    if(load_boot2()) {
        puts("nLoader: Loading complete, launching image.");
        unsigned dummy;
        __asm volatile(
                       "0: mrc p15, 0, r15, c7, c10, 3 @ test and clean DCache \n"
                       " bne 0b \n"
                       " mov %0, #0 \n"
                       " mcr p15, 0, %0, c7, c7, 0 @ invalidate ICache and DCache \n" : "=r" (dummy));
        asm(
            ".arm \n"
            "ldr pc, =0x11800000 \n"
            );
    } else {
        puts("nLoader: Error reading/validating BOOT2 image");
        draw_error(); // boot1.5 does not do this
    }
    while(1);
    __builtin_unreachable();
}
