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
#include "utils.h"
#include "screen.h"
#include "error.h"
#include "bar.h"

uint16_t* SCREEN_BASE_ADDR = 0;

static int CR4 = 0;

void init_screen() {
	SCREEN_BASE_ADDR = *(uint16_t**)SCREEN_BASE_PTR;
	CR4 = iscr4();
}

void drawImage(uint16_t* buf, int bufwidth, int bufheight, int shiftx, int shifty, int trsp) {
	if(!CR4) {
		for(int y = 0; y < bufheight; y++) {
			for(int x = 0; x < bufwidth; x++){
				if(!trsp || buf[y*bufwidth+x] != MAGENTA) {
					SCREEN_BASE_ADDR[(shifty+y)*SCREEN_WIDTH+(shiftx+x)]=(buf[y*bufwidth+x]);
				}
			}
		}
	} else {
		for(int y = 0; y < bufheight; y++) {
			for(int x = 0; x < bufwidth; x++){
				if(!trsp || buf[y*bufwidth+x] != MAGENTA) {
					SCREEN_BASE_ADDR[(shiftx+x)*SCREEN_HEIGHT+(shifty+y)]=(buf[y*bufwidth+x]);
				}
			}
		}
	}
}

void drawBarImage(int bufwidth, int shiftx, int shifty) {
	if(!CR4) {
		for(int y = 0; y < 12; y++) {
			for(int x = 0; x < bufwidth; x++){
				SCREEN_BASE_ADDR[(shifty+y)*SCREEN_WIDTH+(shiftx+x)]=(bar_216x12[y*216+x]);
			}
		}
	} else {
		for(int y = 0; y < 12; y++) {
			for(int x = 0; x < bufwidth; x++){
				SCREEN_BASE_ADDR[(shiftx+x)*SCREEN_HEIGHT+(shifty+y)]=(bar_216x12[y*216+x]);
			}
		}
	}
}

void draw_error() {
	drawImage(error,30,30,(SCREEN_WIDTH-30)/2, SCREEN_HEIGHT-96,1);
}

