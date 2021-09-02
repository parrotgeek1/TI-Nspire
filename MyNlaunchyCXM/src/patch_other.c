/*
 * nLaunchy
 *
 * Copyright (C) 2012-2013 nLaunch team
 * Copyright (C) 2013      nLaunch CX guy
 * Copyright (C) 2013-2015 Excale
 * Copyright (C) 2013      Lionel Debroux
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

#if NDLESS
    #include "ndless.c"
#endif

static inline unsigned char asicflags() {
	return ((*(volatile unsigned int*) 0x900A002C)>>26)&0b11111;
}

static __attribute__((always_inline)) void patch_CAS_1_1_9170_extra(void)      {
	put_word(0xA4012EB4, 0x01012315); // 1.1.8981
}
static __attribute__((always_inline)) void patch_NCAS_1_1_9253_extra(void)     {
	put_word(0xA4012EB4, 0x01012315); // 1.1.8981
}

static __attribute__((always_inline)) void patch_CAS_1_7_2741_extra(void)      { }
static __attribute__((always_inline)) void patch_NCAS_1_7_2741_extra(void)     { }

static __attribute__((always_inline)) void patch_CAS_2_0_1_60_extra(void)      { }
static __attribute__((always_inline)) void patch_NCAS_2_0_1_60_extra(void)     { }

static __attribute__((always_inline)) void patch_CAS_2_1_0_631_extra(void)     { }
static __attribute__((always_inline)) void patch_NCAS_2_1_0_631_extra(void)    { }

static __attribute__((always_inline)) void patch_CAS_3_0_1_1753_extra(void)    {
    put_byte(0x100791A3, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_0_1_1753_extra(void)   {
    put_byte(0x10079243, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_0_2_1791_extra(void)    {
    put_byte(0x100791A3, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_0_2_1791_extra(void)   {
    put_byte(0x10079243, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_1_0_392_extra(void)     {
    #if NDLESS
    write_ndless3x_loader(1, 0);
    #endif
    put_word(0x100B19EC, 0xEAFFFFFE);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_1_0_392_extra(void)    {
    #if NDLESS
    write_ndless3x_loader(0, 0);
    #endif
    put_word(0x100B172C, 0xEAFFFFFE);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_2_0_1212_extra(void)    {
    #if OVERCLOCK
    put_word(0x10000258, NOP);
    #endif
    put_byte(0x10084A63, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_2_0_1212_extra(void)   {
    #if OVERCLOCK
    put_word(0x10000258, NOP);
    #endif
    put_byte(0x10084B13, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_2_0_1219_extra(void)    {
    #if OVERCLOCK
    put_word(0x10000258, NOP);
    #endif
    put_byte(0x10084A63, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_2_0_1219_extra(void)   {
    #if OVERCLOCK
    put_word(0x10000258, NOP);
    #endif
    put_byte(0x10084B13, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_2_3_1233_extra(void)    {
    #if OVERCLOCK
    put_word(0x10000268, NOP);
    #endif
    put_byte(0x10084E8B, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_2_3_1233_extra(void)   {
    #if OVERCLOCK
    put_word(0x10000268, NOP);
    #endif
    put_byte(0x10084F3B, 0xEA);
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_6_0_546_extra(void) {
    #if NDLESS
    write_ndless3x_loader(1, 1);
    #else
    put_word(0x100BEB6C, 0xE12FFF1E); // boot2upd
    #endif
    put_byte(0x1008E157, 0xEA);
    put_word(0x10136418, 0xE12FFF1E); // self destroy
	if(asicflags() != 1) {
		put_word(0x100C0CED+0x17,0xE3A04341); // CAS ASIC byte #2
		put_word(0x100C0CED+0x48B,0x00010105); // CAS ASIC byte #1
		put_byte(0x100C0CED,0x54);
		put_word(0x100C0CED+0x1B,0);
	}
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}
static __attribute__((always_inline)) void patch_NCAS_3_6_0_546_extra(void) {
    #if NDLESS
    write_ndless3x_loader(0, 1);
    #else
    put_word(0x100BE8B4, 0xE12FFF1E);
    #endif
    put_byte(0x1008E1FB, 0xEA);
    put_word(0x10135DF4, 0xE12FFF1E);
	if(asicflags() != 0) {
		put_word(0x100C0A35+0x17,0xE3A04000); // num ASIC byte #2
		put_word(0x100C0A35+0x48B,0); // num ASIC byte #1
		put_byte(0x100C0A35,0x54);
		put_word(0x100C0A35+0x1B,0);
	}
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

static __attribute__((always_inline)) void patch_CAS_3_9_0_46x_extra(void) {
    #if NDLESS
    write_ndless3x_loader(1, 2);
    #else
    put_word(0x100BE598, 0xE12FFF1E); // boot2upd
    #endif
    put_byte(0x1008817B, 0xEA);
    put_word(0x10111584, NOP);
    put_word(0x1013C894, 0xE12FFF1E); // self destroy
	if(asicflags() != 1) {
		put_word(0x100C0719+0x17,0xE3A04341); // CAS ASIC byte #2
		put_word(0x100C0719+0x48B,0x00010105); // CAS ASIC byte #1
		put_byte(0x100C0719,0x54);
		put_word(0x100C0719+0x1B,0);
            // fake as product ID (0x804) 0C. it is close to the touchpad 806 patch
            put_word(0x100c131c, 0xE3A0000C); // FIXME: port to other OSes
	}
    put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
#ifdef TPEVT1 // FIXME: port to other OSes
    // Scale 310 units = 1 volt
    // A value from 0 to 20 indicates normal TI-Nspire keypad.
    // A value from 21 to 42 indicates TI-84+ keypad.
    // According to nspire_emu, SimpKey prototype value is ~52.
    // A value around 73 indicates a TI-Nspire with touchpad
    // adc - look for c4000120 and then +0x10 relative, change the +0x10 load to hardcode 52
    put_word(0x100CDDA8,0xE3A04034); // 34 is the fake adc value (=52 dec=SimpKey/EVT)
	// FIXME: ndless won't work with ID 3
    // manuf revision 0x806 = 2 - without this, it doesn't even check the ADC, just hardcodes clickpad
	// ldr lr, =0x806 / ldr r1, =0x804
    // and then later, ldrh r12, [r5,lr]
    // replace the ldrh with mov r12, #2 = e3a0c002
    put_word(0x100C1308,0xE3A0C002);
#endif
}
static __attribute__((always_inline)) void patch_NCAS_3_9_0_46x_extra(void) {
    #if NDLESS
    write_ndless3x_loader(0, 2);
    #else
    put_word(0x100BE7D0, 0xE12FFF1E); // boot2upd
    #endif
    put_byte(0x1008870B, 0xEA);
    put_word(0x10111788, NOP);
    put_word(0x1013C764, 0xE12FFF1E); // self destroy 3.9.0.463
	if(asicflags() != 0) {
		put_word(0x100C0951+0x17,0xE3A04000); // num ASIC byte #2
		put_word(0x100C0951+0x48B,0); // num ASIC byte #1
		put_byte(0x100C0951,0x54);
		put_word(0x100C0951+0x1B,0);
	}
	put_word(0xA4012EB4, 0x03010083); // 3.0.1.131
}

