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

#ifndef _SCREEN_H
#define _SCREEN_H

#include "types.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define SCREEN_BASE_PTR	0xC0000010
extern uint16_t* SCREEN_BASE_ADDR;

#define MAGENTA	0xF81F

void initScreen();
void drawImage(uint16_t* buf, int bufwidth, int bufheight, int shiftx, int shifty, int trsp);
void drawError();
void drawBarImage(int bufwidth, int shiftx, int shifty);
#endif
