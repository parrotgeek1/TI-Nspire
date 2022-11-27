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
#include "screen.h"

int iscr4() {
    return (*(volatile uint32_t *)0xa4012eb0 >= 0x04000000); // i.e. boot1 >= 4.0
}

// memcpy is now in .S

void ut_disable_watchdog(void) {
    // Disable the watchdog on CX that may trigger a reset
    *(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
    *(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
    *(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}

void putsn(char *str) {
    volatile unsigned *line_status_reg = (unsigned*)0x90020018;
    volatile unsigned *xmit_holding_reg = (unsigned*)0x90020000;
    while(*str) {
        while(!(*line_status_reg & 0b10000000)); // wait for empty xmit holding reg
        *xmit_holding_reg = *str++;
    }
    while(!(*line_status_reg & 0b10000000)); // wait for empty xmit holding reg
}

void puts(char *str) {
    putsn(str);
    putsn("\r\n");
}

void abort() {
    draw_error();
    puts("ABORT");
    while(1);
}

// what? needed for linking, even with -fno-exceptions
void __aeabi_unwind_cpp_pr1() {}
