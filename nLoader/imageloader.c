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
#include "utils.h"
#include "imageloader.h"
#include "screen.h"

/* Load TI boot image files */

#define ISVALIDFIELD(field) (((field)>=0x8000 && (field)<0x9000) || ((field)>=0x0200 && (field)<0x0400))

uint8_t getbyte(uint8_t** p) {
	uint8_t byte = *(*p);
	*p=(*p)+1;
	return byte;
}

uint16_t gethalfword(uint8_t** p) {
	uint8_t hi = getbyte(p);
	return hi << 8 | getbyte(p);
}

uint32_t getword(uint8_t** p) {
	uint16_t hi = gethalfword(p);
	return hi << 16 | gethalfword(p);
}

uint32_t getbits(uint32_t n, uint8_t** p) {
	static uint32_t buf = 0, bits = 0;
	if(!n) {
		buf=0;
		bits=0;
	}
	while (bits < n) {
		buf = buf << 8 | getbyte(p);
		bits += 8;
	}
	bits -= n;
	return buf >> bits & ((unsigned)((1 << n) - 1));
}

uint32_t decompress(uint8_t** p, uint8_t** outp) {
	uint32_t size = getword(p);
	const uint32_t osize = size;
	int CR4 = isCXCR4Hardware();
	const int pixel_start = (CR4 ? 72 : 163); // not sure about this on CR4; should be 1 more than when exploit starts
	const int pixel_end = (CR4 ? 159 : 181); // verified on CR4 and non-CR4
	uint16_t common[64];
	int i;
	int pixel = 0;
	int oldpixel = 1;

	for (i = 0; i < 64; i++)
		common[i] = gethalfword(p);

	for (; size > 0; size -= 2) {
		uint16_t hw;
		if (getbits(1,p))
			hw = (uint16_t)getbits(16,p);
		else
			hw = common[(uint16_t)getbits(6,p)];
		**outp = (uint8_t)(hw >> 8);
		(*outp)++;
		**outp = (uint8_t)hw;
		(*outp)++;
		if((size % 8192) == 0)  {
			pixel = 1+pixel_start-52+(int)(((double)(osize-size)/(double)osize)*(pixel_end-pixel_start));
			if(pixel != oldpixel) {
				drawBarImage(pixel,52,121);
				oldpixel = pixel;
			}
		}
	}
	getbits(0,0);
	return size;
}

void* loadTIBootImage(void* buf) {
	uint8_t* p = (uint8_t *)buf;
	uint32_t flags = 0;
	uint8_t* outp;
	uint8_t* outbuf = 0;
	uint32_t addr = 0;
	while (1) {
		uint16_t field = gethalfword(&p);
		uint32_t size = field & 0x000F;
		field &= 0xFFF0;
		if(ISVALIDFIELD(field)) {
			if (size == 0x0D)      size = getbyte(&p);
			else if (size == 0x0E) size = gethalfword(&p);
			else if (size == 0x0F) size = getword(&p);
		}
		if (field == 0x8000) {
			/* Don't skip - 8070 is inside this */
		} else if (field == 0x8080) {
			if (size < 8) {
				return (uint8_t *)0;
			}
			addr = getword(&p);
			outbuf = (uint8_t*)addr;
			memcpy(&flags,p,4);
			p+=4;
		} else if (field == 0x8070) {
			if(!outbuf) {
				return 0;
			}
			if (flags) {
				outp = outbuf;
				decompress(&p,&outp);
				break;
			} else {
				memcpy(outbuf,p,size); /* this means no progress bar if it's uncompressed, but that is unlikely */
				p+=size;
				outp=outbuf+size;
				break;
			}
		} else if (field == 0xFFF0) {
			/* End of image */
			break;
		} else {
			/* Skip it */
			p+=size;
		}
	}
	return (void *)addr;
}
