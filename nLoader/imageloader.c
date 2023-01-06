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
/* Decompression for boot2 */

#define ISVALIDFIELD(field) (((field)>=0x8000 && (field)<0x9000) || ((field)>=0x0200 && (field)<0x0400))

unsigned char getbyte(char** p) {
	int byte = *(*p);
	*p=(*p)+1;
	if (byte < 0) {
		puts("boot2 decompression EOF reached unexpectedly");
		draw_error();
		while(1);
	}
	return byte;
}

unsigned short gethalfword(char** p) {
	unsigned char hi = getbyte(p);
	return hi << 8 | getbyte(p);
}

unsigned int getword(char** p) {
	unsigned short hi = gethalfword(p);
	return hi << 16 | gethalfword(p);
}

int getbits(unsigned int n, char** p) {
	static unsigned int buf = 0, bits = 0;
	if(!n) {
		buf=0;
		bits=0;
	}
	while (bits < n) {
		buf = buf << 8 | getbyte(p);
		bits += 8;
	}
	bits -= n;
	return buf >> bits & ((1 << n) - 1);
}

unsigned int decompress(char** p, char** outp) {
	unsigned int size = getword(p);
	const unsigned int osize = size;
	int CR4 = iscr4();
	const int pixel_start = (CR4 ? 72 : 163); // not sure about this on CR4; should be 1 more than when exploit starts
	const int pixel_end = (CR4 ? 159 : 181); // verified on CR4 and non-CR4
	
	unsigned short common[64];
	int i;
	int pixel = 0;
	int oldpixel = 1;
	
	for (i = 0; i < 64; i++)
		common[i] = gethalfword(p);
	
	for (; size > 0; size -= 2) {
		unsigned short hw;
		if (getbits(1,p))
			hw = getbits(16,p);
		else
			hw = common[getbits(6,p)];
		**outp = hw >> 8;
		(*outp)++;
		**outp = hw;
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


int decompressFiles(char* buf, char* outbuf)
{	char* p = buf;
	unsigned int flags=0;
	char* outp=outbuf;
	unsigned int addr=0;
	int size8000=0;
	char* data8000=0;
	while (1) {
		unsigned short field = gethalfword(&p);
		unsigned int size = field & 0x000F;
		field &= 0xFFF0;
		if(ISVALIDFIELD(field)) {
			if (size == 0x0D)      size = getbyte(&p);
			else if (size == 0x0E) size = gethalfword(&p);
			else if (size == 0x0F) size = getword(&p);
		}
		if (field == 0x8000) {
			/* Don't skip - 8070 is inside this */
			size8000=size;
			data8000=p;
		}
		else if (field == 0x8080) {
			if (size < 8) {
				return 0;
			}
			addr = getword(&p);
			if(addr != 0x11800000) { /* protect from potential crash loop */
				return 0;
			}
			memcpy(&flags,p,4);
			p+=4;
		} else if (field == 0x8070) {
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
			break;
		} else {
			/* Skip it */
			p+=size;
		}
	}
	return outp-(outbuf);
}
