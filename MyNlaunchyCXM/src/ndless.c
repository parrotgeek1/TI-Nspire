/*
 * nLaunchy
 *
 * Copyright (C) 2012-2013 nLaunch team
 * Copyright (C) 2013      nLaunch CX guy
 * Copyright (C) 2013-2015 Excale
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

static void write_ndless3x_loader(uint32_t model, uint32_t os) {
    // model: 0=non-CAS ; 1=CAS
    // os: 0=3.1.0.392 ; 1=3.6.0.546 ; 2=3.9.x ; 3=4.0.3.29 ; 4=4.2.0.532
    #define OFFSET_MODEL 0x33
    static const uint32_t ndless3x_load_addr[][2] = {
        {0x100A6A7C, 0x100A6D3C},
        {0x100BE8B4, 0x100BEB6C},
        {0x100BE7D0, 0x100BE598}
    };
    static uint32_t ndless3x_loader[] = {
        0xE92D4030, 0xE59F30B0, 0xE59F20B0, 0xE59F50B0,
        0xE59F10B0, 0xE5832C00, 0xE08F5005, 0xE3A02000,
        0xE5832008, 0xE24DD02C, 0xE5832C00, 0xE1A00005,
        0xE08F1001, 0xE59F3090, 0xE12FFF33, 0xE2504000,
        0x0A00001A, 0xE1A0100D, 0xE1A00005, 0xE59F307C,
        0xE12FFF33, 0xE59D0014, 0xE59F3074, 0xE12FFF33,
        0xE59D1014, 0xE3A02001, 0xE1A03004, 0xE59FC064,
        0xE1A05000, 0xE12FFF3C, 0xE1A00004, 0xE59F3058,
        0xE12FFF33, 0xE59F3054, 0xE28D1028, 0xE08F3003,
        0xE5213004, 0xEE17FF7A, 0x1AFFFFFD, 0xE3A03000,
        0xEE073F17, 0xE3A00001, 0xE2855004, 0xE12FFF35,
        0xE3A00000, 0xE28DD02C, 0xE8BD8030, 0x90060000,
        0x1ACCE551, 0x000000C4, 0x000000D7, 0xAAAAAAAA,
        0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE,
        0x0000007E, 0x636F642F, 0x6E656D75, 0x6E2F7374,
        0x73656C64, 0x646E2F73, 0x7373656C, 0x7365725F,
        0x6372756F, 0x742E7365, 0x0000736E, 0x72000000,
        0x004C0062, 0x00000000, 0x00000000, 0x00000000
    };
    static const uint32_t ndless3x_loader_model_specific[][2][5] = {
        {{0x1037A020, 0x10382058, 0x103748F8, 0x1037A1C4, 0x10379894},
         {0x1037A750, 0x10382788, 0x10375028, 0x1037A8F4, 0x10379FC4}},
        {{0x103FE9D4, 0x10406ACC, 0x103F8F6C, 0x103FEB78, 0x103FE248},
         {0x103FF414, 0x1040750C, 0x103F99AC, 0x103FF5B8, 0x103FEC88}},
        {{0x10405C78, 0x1040DEF0, 0x1040033C, 0x10405E1C, 0x10405498},
         {0x104061B8, 0x1040E430, 0x1040087C, 0x1040635C, 0x104059D8}}
    };

    for (uint32_t i = 0; i < sizeof(ndless3x_loader_model_specific[0][0])/sizeof(ndless3x_loader_model_specific[0][0][0]); i++) {
        ndless3x_loader[i+OFFSET_MODEL] = ndless3x_loader_model_specific[os][model][i];
    }
    uint32_t * dest = (uint32_t *)ndless3x_load_addr[os][model];
    for (uint32_t i = 0; i < sizeof(ndless3x_loader)/sizeof(ndless3x_loader[0]); i++) {
        *dest++ = ndless3x_loader[i];
    }
}
