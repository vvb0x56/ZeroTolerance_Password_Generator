/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>


#define DEFAULT_BUF_SIZE        32
#define AGGREGATE_BUF_SIZE      12 // 5 - gun, 5 - ammo, 1 - health, 1 - level
#define INTERMEDIATE_BUF_SIZE   7
#define ENCRYPT_BUF_SIZE        7
#define PASSWORD_LENGTH         9
#define GUN_SLOTS               5
#define AMMO_SLOTS              5
#define PLAYERS_NUM             5
#define HEALTH                  1
#define LEVEL                   1

#define PRS_SATOE_ISHII       0x0
#define PRS_SCOTT_HAILE       0x1
#define PRS_JUSTIN_WOLF       0x2
#define PRS_TONY_RAMOS        0x3
#define PRS_TOMAS_GJOERUP     0x4

#define GUN_START             0x1
#define GUN_BIO_SCANNER       0x1
#define GUN_MINE              0x2
#define GUN_BOOLET_PROOF_VEST 0x3
#define GUN_FIRE_EXTINGUISHER 0x4
#define GUN_FIRE_PROOF_SUIT   0x5
#define GUN_FLASH_LIGTH       0x6
#define GUN_HANDGUN           0x8
#define GUN_NIGHT_VISION      0x9
#define GUN_LASER_AIMED_GUN   0xa
#define GUN_ROCKET_LAUNCHER   0xb
#define GUN_SHOUTGUN          0xc
#define GUN_FLAME_THROWER     0xd
#define GUN_PULSE_GUN         0xe
#define GUN_END               0xf

#define LVL_DOCKING_BAY_2                       0x0 
#define LVL_BRIDGE_1                            0x1 
#define LVL_ENGINEERING_1                       0x2 
#define LVL_ENGINEERING_2                       0x3 
#define LVL_ENGINEERING_3                       0x4 
#define LVL_ENGINEERING_4                       0x5 
#define LVL_GREEN_HOUSE_1                       0x6 
#define LVL_GREEN_HOUSE_2                       0x7 
#define LVL_GREEN_HOUSE_3                       0x8 
#define LVL_BRIDGE_2                            0x9 
#define LVL_REACTOR_1                           0xa 
#define LVL_REACTOR_2                           0xb 
#define LVL_FLOOR_164                           0x10
#define LVL_FLOOR_163                           0x11
#define LVL_FLOOR_162                           0x12
#define LVL_FLOOR_161                           0x13
#define LVL_FLOOR_160                           0x14
#define LVL_FLOOR_159                           0x15
#define LVL_FLOOR_158                           0x16
#define LVL_FLOOR_157                           0x17
#define LVL_FLOOR_156                           0x18
#define LVL_FLOOR_155                           0x19
#define LVL_FLOOR_154                           0x1A
#define LVL_FLOOR_153                           0x1b
#define LVL_FLOOR_152                           0x1c
#define LVL_FLOOR_151                           0x1d
#define LVL_BASEMENT_1                          0x1f
#define LVL_BASEMENT_2                          0x20
#define LVL_BASEMENT_3                          0x21
#define LVL_BASEMENT_4                          0x22
#define LVL_BASEMENT_5                          0x23
#define LVL_BASEMENT_6                          0x24
#define LVL_BASEMENT_7                          0x25
#define LVL_BASEMENT_8                          0x26
#define LVL_BASEMENT_9                          0x27
#define LVL_BASEMENT_10                         0x28
#define LVL_BASEMENT_11                         0x29
#define LVL_BASEMENT_12                         0x2a
#define LVL_ROOFTOP                              0xf 

typedef unsigned char   Byte; // 8  bit
typedef unsigned short  Word; // 16 bit
typedef unsigned int    Long; // 32 bit

struct slot_t {
    Byte        gun;
    Word        ammo;
};

struct globalArgs_t {
    Byte health;
    Byte level;
    struct slot_t equipment[GUN_SLOTS];
    Byte alive_persons[PLAYERS_NUM];
} globalArgs;


const Word MAX_AMMO_FOR_GUN[] = { 0x0100,     
                                  0x0a00,     // 0x1 - Bio Scanner        0xb 
                                  0x6300,     // 0x2 - Mine               0x6300
                                  0x0a00,     // 0x3 - Boolet Proof Vest  0xb
                                  0x0a00,     // 0x4 - Fire Extinguisher  0xb 
                                  0x0a00,     // 0x5 - Fire Proof Suit    0xb 
                                  0x0a00,     // 0x6 - Flash Ligth        0xb
                                  0x6300,     // 0x8 - Handgun            0x6300
                                  0x6300,     // 0x9 - Night Vision       0xb
                                  0x0a00,     // 0xa - Laser Aimed Gun    0x63FF
                                  0x6300,     // 0xb - Rocket Launcher    0x6300 
                                  0x6300,     // 0xc - Shoutgun           0x6300
                                  0x6300,     // 0xd - Flame Thrower      0xb
                                  0x0a00,     // 0xe - Pulse Gun          0x6300
                                  0x6300  };

const Byte SOME_ENCRYPT_DICT[]  = { 0x2e, 0x28, 0x16, 0x5,  0x12, 0xe,  0x2f, 0x26, 
                                    0x13, 0x1d, 0x6,  0xf,  0x30, 0x29, 0x27, 0x1e, 
                                    0x7,  0x10, 0x31, 0x2a, 0x25, 0x17, 0x8,  0x11, 
                                    0x22, 0x14, 0x18, 0x1f, 0x9,  0x0,  0x32, 0x2b, 
                                    0x19, 0x20, 0xa,  0x1,  0x33, 0x23, 0x1a, 0x21, 
                                    0xb,  0x2,  0x34, 0x2c, 0x15, 0x1b, 0xc,  0x3, 
                                    0x35, 0x2d, 0x24, 0x1c, 0xd,  0x4,  0x48, 0xa7 };

const Byte ACCEPTED_CHARS[] = {     0x78, 0x42, 0x3F, 0x44, 0x45, 0x6A, 0x47, 0x48, 
                                    0x29, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 
                                    0x52, 0x53, 0x54, 0x55, 0x56, 0x51, 0x57, 0x58, 
                                    0x59, 0x5A, 0x61, 0x62, 0x65, 0x66, 0x63, 0x64, 
                                    0x68, 0x69, 0x67, 0x46, 0x6B, 0x21, 0x6D, 0x6E, 
                                    0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 
                                    0x77, 0x79, 0x41, 0x7A, 0x32, 0x37, 0x34, 0x35, 
                                    0x33, 0x39, 0x36, 0x38, 0x43, 0x2F, 0x2D, 0x2A }; 



const Byte BCLR_BYTE[] = {         0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };
const Byte BSET_BYTE[] = {         0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

const Long BSET_UINT[] = {         0x00000001, 0x00000002, 0x00000004, 0x00000008,
                                   0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                   0x00000100, 0x00000200, 0x00000400, 0x00000800,
                                   0x00001000, 0x00002000, 0x00004000, 0x00008000,
                                   0x00010000, 0x00020000, 0x00040000, 0x00080000,
                                   0x00100000, 0x00200000, 0x00400000, 0x00800000,
                                   0x01000000, 0x02000000, 0x04000000, 0x08000000,
                                   0x10000000, 0x20000000, 0x40000000, 0x80000000  };

const Long ASR_USHORT[] = {        0x0000, 0x8000, 0xC000, 0xE000,
                                   0xF000, 0xF800, 0xFC00, 0xFE00,
                                   0xFF00, 0xFF80, 0xFFC0, 0xFFE0,
                                   0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF  };



Word ror_16(Word n, Word d) {
    return (n >> d) | (n << (sizeof(n) * 8 - d));
}

Word rol_16(Word n, Word d) {
    return (n << d) | (n >> (sizeof(n) * 8 - d));
}

Long ror_32(Long n, Long d) {
    return (n >> d) | (n << (sizeof(n) * 8 - d));
}

Word asr_16(Word n, Word d) {
    return (n >> d) | ( n & 0x80000000 ? ASR_USHORT[d] : 0x0);
}

Long swap_32(Long n) {
    return ror_32(n, sizeof(n) * 8 / 2);
}


void print_buf(const char *buf_name, Byte *buf, int buf_size) {
    printf("<%s> [ ", buf_name);
    for (int i = 0; i < buf_size; i++)  printf("%x ", buf[i]);
    printf("]\n"); 
     
}


// @
void magic_5868a(Word *d0, Word d2, Byte *a2) {
    Word d1;
    *d0 = ror_16(*d0, 1);       // ror.w #1, d0

    
    d2 &= 0x00FF;                                   //addi.w #$FF,d2
    d1 = d2;

    d1 &= 0x0007;           // andi.w #7,d1 // % 8
    d2 >>= 3;
    
    //!!! UNIT ? 0x0005869c             btst.l  d1,(a2,d2.w)
    if (d1 >= 8)    printf("WARN d1(%x) is bigger than 8\n", d1);

    if ( (a2[d2] & BSET_BYTE[d1]) != 0) {   
        *d0 |= 0x0001; //   bset.b #0,d0 
    }
    else {
        *d0 &= 0xFFFE; //   bclr.b #0,d0
    }
}
// @ d2++;


// d0 == index in list
// d1 == transform counter
// a1 == result array
void magic_58660(Word d0, Word d1, Byte* a1) {
    d1 &= 0xFF; 

    if (d0 % 2)     a1[d1 / 8] |= BSET_BYTE[d1 % 8];
    else            a1[d1 / 8] &= BCLR_BYTE[d1 % 8];

    return;
}
// @ d1++
// @ ror_16(d0, 1)




void init(Byte *aggr_buf, Byte *inter_buf, Byte *encrypt_buf) {

    memset(aggr_buf,    0, AGGREGATE_BUF_SIZE);
    memset(inter_buf,   0, INTERMEDIATE_BUF_SIZE);
    memset(encrypt_buf, 0, ENCRYPT_BUF_SIZE);
    
    globalArgs.health = 100;
    globalArgs.level = LVL_DOCKING_BAY_2;
    
    globalArgs.equipment[0].gun = GUN_PULSE_GUN;
    globalArgs.equipment[0].ammo = 0x6300;
    globalArgs.equipment[1].gun = GUN_SHOUTGUN;
    globalArgs.equipment[1].ammo = 0x6300;
    globalArgs.equipment[2].gun = GUN_ROCKET_LAUNCHER;
    globalArgs.equipment[2].ammo = 0x6300;
    globalArgs.equipment[3].gun = GUN_BOOLET_PROOF_VEST;
    globalArgs.equipment[3].ammo = 0x0b00;
    globalArgs.equipment[4].gun = GUN_LASER_AIMED_GUN;
    globalArgs.equipment[4].ammo = 0x6300;
    globalArgs.alive_persons[0] = 1;
    globalArgs.alive_persons[1] = 1;
    globalArgs.alive_persons[2] = 1;
    globalArgs.alive_persons[3] = 1;
    globalArgs.alive_persons[4] = 1;

}



void set_guns_to_aggregate_buf(struct slot_t *equipment, Byte *aggr_buf) {
    for (int i = 0; i < GUN_SLOTS; i++)        aggr_buf[i] = equipment[i].gun;
}


Byte calculate_ammo_multiplier(struct slot_t equipment) {
    Long ammo = ((Long) equipment.ammo << 3);

    if ((Word)ammo != 0)     ammo -= 1;

    return (Byte) (ammo / MAX_AMMO_FOR_GUN[equipment.gun]);
}


void set_ammo_to_aggregate_buf(struct slot_t *equipment, Byte *aggr_buf) {
    Byte gun = GUN_START;
    int idx = GUN_SLOTS; // we'll insert after guns in aggr_buf
    while (gun != GUN_END) {
        for (int i = 0; i < GUN_SLOTS; i++) {
            if (equipment[i].gun == gun) {
                aggr_buf[idx] = calculate_ammo_multiplier(equipment[i]);
                idx++;
                break;
            }
        }
        gun++;
    }
}


void set_health_to_aggregate_buf(Long health, Byte *aggr_buf) {
    health <<= 6;

    if (health > 0) health -= 1;

    aggr_buf[GUN_SLOTS + AMMO_SLOTS] = (Byte) (health / 0x64);
}


void set_level_to_aggregate_buf(Byte level, Byte *aggr_buf) {
    aggr_buf[GUN_SLOTS + AMMO_SLOTS + HEALTH] = level;
}


void pack_players_to_intermediate_buf(Byte *alive_persons, Byte *inter_buf, Word *global_pack_index){
    //set alive players to working buf - 28fe
    for (int i = 0; i < PLAYERS_NUM; i++) {
        Byte curr_person = alive_persons[i];
        magic_58660(curr_person, *global_pack_index, inter_buf);    *global_pack_index += 1;
    }
}


void pack_guns_to_intermediate_buf(Byte *aggr_buf, Byte *inter_buf, Word *global_pack_index) {
    Word tmp_pack_code = 0x0;
    for (int i = 0; i < GUN_SLOTS; i++) {
        tmp_pack_code |= BSET_UINT[aggr_buf[i]];  // aggr_buf[i] < 0xf
#ifdef DEBUG
        printf(" tmp_pack_code: %x ", tmp_pack_code);
#endif
    }
#ifdef DEBUG
    printf("\n");
#endif

    tmp_pack_code = asr_16(tmp_pack_code, 1);
#ifdef DEBUG
    printf(" tmp_pack_code: %x\n", tmp_pack_code); 

    printf(" *global_pack_index: %x\n", *global_pack_index); 
#endif
    for (int i = 0; i < 14; i++) {
        magic_58660(tmp_pack_code, *global_pack_index, inter_buf);     
        tmp_pack_code = ror_16(tmp_pack_code, 1);
        *global_pack_index += 1;
    }
}


void pack_ammo_to_intermediate_buf(Byte *aggr_buf, Byte *inter_buf, Word *global_pack_index) {
    Word tmp_ammo;

    for (int ammo = 0; ammo < AMMO_SLOTS; ammo++) {
        tmp_ammo = aggr_buf[GUN_SLOTS + ammo];
    
        magic_58660(tmp_ammo, *global_pack_index, inter_buf); tmp_ammo = ror_16(tmp_ammo, 1); *global_pack_index += 1;
        magic_58660(tmp_ammo, *global_pack_index, inter_buf); tmp_ammo = ror_16(tmp_ammo, 1); *global_pack_index += 1;
        magic_58660(tmp_ammo, *global_pack_index, inter_buf); tmp_ammo = ror_16(tmp_ammo, 1); *global_pack_index += 1;
    }
}


void pack_health_to_intermediate_buf(Byte *aggr_buf, Byte *inter_buf, Word *global_pack_index) {
    Word health_tmp = aggr_buf[GUN_SLOTS + AMMO_SLOTS]; // health  
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
    magic_58660(health_tmp, *global_pack_index, inter_buf); health_tmp = ror_16(health_tmp, 1); *global_pack_index += 1;
}


void pack_level_to_intermediate_buf(Byte *aggr_buf, Byte *inter_buf, Word *global_pack_index) {
    Word level_tmp = aggr_buf[GUN_SLOTS + AMMO_SLOTS + HEALTH]; // level
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
    magic_58660(level_tmp, *global_pack_index, inter_buf); level_tmp = ror_16(level_tmp, 1); *global_pack_index += 1;
}




Word get_checksum_multiplier(Byte *inter_buf) {
    Word tmp_multiplier;
    Word idx = 0x0;
    Word result_multiplier = 0x0;

    for (int i = 0; i < 5; i++) {
        tmp_multiplier = 0x0;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
        tmp_multiplier = rol_16(tmp_multiplier, 7);
        result_multiplier += tmp_multiplier;
    }

    tmp_multiplier = 0x0;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    magic_5868a(&tmp_multiplier, idx, inter_buf);      idx++;
    tmp_multiplier = rol_16(tmp_multiplier, 5);

    result_multiplier += tmp_multiplier;

    return  result_multiplier;

}



Word add_checksum_to_inter_buf(Byte* inter_buf, Word checksum_multiplier, Word *global_pack_index) {
    for (int i = 0; i < 8; i++) {
        magic_58660(checksum_multiplier, *global_pack_index, inter_buf); 
        checksum_multiplier = ror_16(checksum_multiplier, 1); 
        *global_pack_index += 1;
    }

    return checksum_multiplier;
}




void fill_data_to_encrypt_buf(Word checksum_multiplier, Byte *inter_buf, Byte *encrypt_buf) {
    const Byte *encrypt_dict = SOME_ENCRYPT_DICT;
    Word idx = 0x0;
    Byte some_byte_from_encrypt_dict;

    for (int i = 0; i < 56; i++) {   // 56 - len of the SOME_ENCRYPT_DICT
        magic_5868a(&checksum_multiplier, idx, inter_buf);      idx++;
        some_byte_from_encrypt_dict = encrypt_dict[i];

        magic_58660(checksum_multiplier, some_byte_from_encrypt_dict, encrypt_buf); 
        checksum_multiplier = ror_16(checksum_multiplier, 1); 
    }
}



void encrypt_encrypt_buf(Byte *encrypt_buf) {
    Long magic_num = 0x56ca2d69;
    Byte tmp_byte;
    
    for (int i = 0; i < ENCRYPT_BUF_SIZE; i++) {
        tmp_byte = encrypt_buf[i];

        tmp_byte ^= (Byte)magic_num;
        magic_num = ror_32(magic_num, 7);
        magic_num = swap_32(magic_num);

        encrypt_buf[i] = tmp_byte;
    }
}



void get_password_from_encrypt_buf(Byte *encrypt_buf, char *password) {
    //char result[PASSWORD_LENGTH + 1] = { 0 };
    const Byte *chars_list = ACCEPTED_CHARS;
    //unsigned char *worki2_result = worki2_buf_2908;

    Word convert_index = 0x0;
    //unsigned short d6 = 0x8;
    Word idx_in_chars_list;
   
    for (int i = 0; i < PASSWORD_LENGTH; i++) { 
        idx_in_chars_list = 0x0;

        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;
        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;
        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;
        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;
        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;
        magic_5868a(&idx_in_chars_list, convert_index, encrypt_buf);      convert_index++;

        idx_in_chars_list = rol_16(idx_in_chars_list, 5);

        password[i] = (char)chars_list[idx_in_chars_list];
    }
}




void create_password(Byte *aggr_buf, Byte *inter_buf, Byte *encrypt_buf, char *password) {
    Byte health = globalArgs.health;
    Byte level  = globalArgs.level;
    Byte *alive_persons = globalArgs.alive_persons;

    struct slot_t *equipment = globalArgs.equipment;

    // fill data to aggregate buf
    //
    set_guns_to_aggregate_buf(equipment, aggr_buf);    
#ifdef DEBUG
    print_buf("aggr_buf guns  ", aggr_buf, AGGREGATE_BUF_SIZE);
#endif 

    set_ammo_to_aggregate_buf(equipment, aggr_buf);
#ifdef DEBUG
    print_buf("aggr_buf ammo  ", aggr_buf, AGGREGATE_BUF_SIZE);
#endif 

    set_health_to_aggregate_buf(health, aggr_buf);
#ifdef DEBUG
    print_buf("aggr_buf health", aggr_buf, AGGREGATE_BUF_SIZE);
#endif 

    set_level_to_aggregate_buf(level, aggr_buf);
#ifdef DEBUG
    print_buf("aggr_buf level ", aggr_buf, AGGREGATE_BUF_SIZE);
#endif 


    // pack player guns ammo health level to intermidiate buf
    //
    Word global_pack_index = 0; 
    pack_players_to_intermediate_buf(alive_persons, inter_buf,  &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf pack persons", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 

    pack_guns_to_intermediate_buf(aggr_buf, inter_buf, &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf pack guns", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 

    pack_ammo_to_intermediate_buf(aggr_buf, inter_buf, &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf pack ammo", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 

    pack_health_to_intermediate_buf(aggr_buf, inter_buf, &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf pack health", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 

    pack_level_to_intermediate_buf(aggr_buf, inter_buf, &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf pack level", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 
    
    // add check sum to intermediate buf
    //
    Word checksum_multiplier = get_checksum_multiplier(inter_buf);
#ifdef DEBUG
    printf("checksum_multiplier: %x\n", checksum_multiplier);
#endif 

    checksum_multiplier = add_checksum_to_inter_buf(inter_buf, checksum_multiplier, &global_pack_index);
#ifdef DEBUG
    print_buf("inter_buf add checksum", inter_buf, INTERMEDIATE_BUF_SIZE);
#endif 

    // encrypt intermediate buf, result write to encrypt buf
    //
    fill_data_to_encrypt_buf(checksum_multiplier, inter_buf, encrypt_buf);
#ifdef DEBUG
    print_buf("encrypt_buf fill", encrypt_buf, ENCRYPT_BUF_SIZE);
#endif 

    encrypt_encrypt_buf(encrypt_buf);
#ifdef DEBUG
    print_buf("encrypt_buf encrypt", encrypt_buf, ENCRYPT_BUF_SIZE);
#endif 

    // convert encrypted buf to password
    get_password_from_encrypt_buf(encrypt_buf, password);
}



void print_usage_message() {
    printf("\n");
    printf("\t--help              This help message\n");
    printf("\t--health            Set health in percent from 10-100. Default 100.\n");
    printf("\t--level             Set level from level list.\n");
    printf("\t--gun[1,2,3,4,5]    Set from guns list below.\n");
    printf("\t--kill [0,1,2,3,4]  Set person to kill from the list, Default all persons alive.\n");
    printf("\n");

    printf("-------------------------------------------------------\n");
    printf("|                         LEVELS                      |\n");
    printf("-------------------------------------------------------\n");
    printf("| Docking Bay 2 |     %2d     | Bridge 1      |   %2d   |\n", LVL_DOCKING_BAY_2, LVL_BRIDGE_1);
    printf("| Engineering 1 |     %2d     | Engineering 2 |   %2d   |\n", LVL_ENGINEERING_1, LVL_ENGINEERING_2);
    printf("| Engineering 3 |     %2d     | Engineering 4 |   %2d   |\n", LVL_ENGINEERING_3, LVL_ENGINEERING_4);
    printf("| Green House 1 |     %2d     | Green House 2 |   %2d   |\n", LVL_GREEN_HOUSE_1, LVL_GREEN_HOUSE_2);
    printf("| Green House 3 |     %2d     | Bridge 2      |   %2d   |\n", LVL_GREEN_HOUSE_3, LVL_BRIDGE_2);
    printf("| Reactor 1     |     %2d     | Reactor 2     |   %2d   |\n", LVL_REACTOR_1, LVL_REACTOR_2);
    printf("| Roof Top      |     %2d     |               |        |\n",  LVL_ROOFTOP);
    printf("| Floor 164     |     %2d     | Floor 163     |   %2d   |\n", LVL_FLOOR_164, LVL_FLOOR_163);
    printf("| Floor 162     |     %2d     | Floor 161     |   %2d   |\n", LVL_FLOOR_162, LVL_FLOOR_161);
    printf("| Floor 160     |     %2d     | Floor 159     |   %2d   |\n", LVL_FLOOR_160, LVL_FLOOR_159);
    printf("| Floor 158     |     %2d     | Floor 157     |   %2d   |\n", LVL_FLOOR_158, LVL_FLOOR_157);
    printf("| Floor 156     |     %2d     | Floor 155     |   %2d   |\n", LVL_FLOOR_156, LVL_FLOOR_155);
    printf("| Floor 154     |     %2d     | Floor 153     |   %2d   |\n", LVL_FLOOR_154, LVL_FLOOR_153);
    printf("| Floor 152     |     %2d     | Floor 151     |   %2d   |\n", LVL_FLOOR_152, LVL_FLOOR_151);
    printf("| Basement 1    |     %2d     | Basement 2    |   %2d   |\n", LVL_BASEMENT_1, LVL_BASEMENT_2);
    printf("| Basement 3    |     %2d     | Basement 4    |   %2d   |\n", LVL_BASEMENT_3, LVL_BASEMENT_4);
    printf("| Basement 5    |     %2d     | Basement 6    |   %2d   |\n", LVL_BASEMENT_5, LVL_BASEMENT_6);
    printf("| Basement 7    |     %2d     | Basement 8    |   %2d   |\n", LVL_BASEMENT_7, LVL_BASEMENT_8);
    printf("| Basement 9    |     %2d     | Basement 10   |   %2d   |\n", LVL_BASEMENT_9, LVL_BASEMENT_10);
    printf("| Basement 11   |     %2d     | Basement 12   |   %2d   |\n", LVL_BASEMENT_11, LVL_BASEMENT_12);
    printf("-------------------------------------------------------\n");
    printf("\n");

    printf("\n");
    printf("-----------------------------\n");
    printf("|           GUNS            |\n");
    printf("-----------------------------\n");
    printf("| Bio Scanner         | %2d  |\n", GUN_BIO_SCANNER);
    printf("| Mine                | %2d  |\n", GUN_MINE);
    printf("| Boolet Proof Vest   | %2d  |\n", GUN_BOOLET_PROOF_VEST);
    printf("| Fire Extinguisher   | %2d  |\n", GUN_FIRE_EXTINGUISHER);
    printf("| Fire Proof Suit     | %2d  |\n", GUN_FIRE_PROOF_SUIT);
    printf("| Flash Light         | %2d  |\n", GUN_FLASH_LIGTH);
    printf("| Handgun             | %2d  |\n", GUN_HANDGUN);
    printf("| Night Vision        | %2d  |\n", GUN_NIGHT_VISION);
    printf("| Laser Aimed Gun     | %2d  |\n", GUN_LASER_AIMED_GUN);
    printf("| Rocket Launcher     | %2d  |\n", GUN_ROCKET_LAUNCHER);
    printf("| Shoutgun            | %2d  |\n", GUN_SHOUTGUN);
    printf("| Flame Thrower       | %2d  |\n", GUN_FLAME_THROWER);
    printf("| Pulse Gun           | %2d  |\n", GUN_PULSE_GUN);
    printf("-----------------------------\n");
    printf("\n");
    printf("----------------------------------------------\n");
    printf("|                   PERSONS                  |\n");
    printf("----------------------------------------------\n");
    printf("| Satoe Ishii   | %1d   | Scot Haile | %1d       |\n", PRS_SATOE_ISHII, PRS_SCOTT_HAILE);
    printf("| Justin Wolf   | %1d   | Tony Ramos | %1d       |\n", PRS_JUSTIN_WOLF, PRS_TONY_RAMOS);
    printf("| Tomas Gjoerup | %1d   |                      |\n", PRS_TOMAS_GJOERUP);
    printf("----------------------------------------------\n");
    printf("\n");
    printf("----------------------------------------------\n");
    printf("|             EMBEDDED PASSWORDS             |\n");
    printf("----------------------------------------------\n");
    printf("|  Highrise!  |   Basement!   |   Boxing!!!  |\n");
    printf("----------------------------------------------\n");
}

void set_health(int health_opt) {

    if (health_opt < 10 || health_opt > 100) health_opt = 100;
    
    globalArgs.health = (health_opt * 0x64) / 100;
}

void set_level(int level_opt) {
    switch (level_opt) {
        case LVL_DOCKING_BAY_2: 
        case LVL_BRIDGE_1:
        case LVL_ENGINEERING_1: 
        case LVL_ENGINEERING_2:
        case LVL_ENGINEERING_3: 
        case LVL_ENGINEERING_4:
        case LVL_GREEN_HOUSE_1: 
        case LVL_GREEN_HOUSE_2:
        case LVL_GREEN_HOUSE_3: 
        case LVL_BRIDGE_2:
        case LVL_REACTOR_1: 
        case LVL_REACTOR_2:
        case LVL_FLOOR_164: 
        case LVL_FLOOR_163:
        case LVL_FLOOR_162: 
        case LVL_FLOOR_161:
        case LVL_FLOOR_160: 
        case LVL_FLOOR_159:
        case LVL_FLOOR_158:
        case LVL_FLOOR_157:
        case LVL_FLOOR_156: 
        case LVL_FLOOR_155:
        case LVL_FLOOR_154: 
        case LVL_FLOOR_153:
        case LVL_FLOOR_152: 
        case LVL_FLOOR_151:
        case LVL_ROOFTOP:
        case LVL_BASEMENT_1: 
        case LVL_BASEMENT_2:
        case LVL_BASEMENT_3: 
        case LVL_BASEMENT_4:
        case LVL_BASEMENT_5: 
        case LVL_BASEMENT_6:
        case LVL_BASEMENT_7: 
        case LVL_BASEMENT_8:
        case LVL_BASEMENT_9: 
        case LVL_BASEMENT_10:
        case LVL_BASEMENT_11: 
        case LVL_BASEMENT_12:
            globalArgs.level = level_opt;
            break;
        default:
            //globalArgs.level = LVL_DOCKING_BAY_2;
            break;
    }
}

void set_equipment(gun_num, gun_opt) {

    switch (gun_opt) {
        case GUN_BIO_SCANNER:
        case GUN_MINE:
        case GUN_BOOLET_PROOF_VEST:
        case GUN_FIRE_EXTINGUISHER:
        case GUN_FIRE_PROOF_SUIT:
        case GUN_FLASH_LIGTH:
        case GUN_HANDGUN:
        case GUN_NIGHT_VISION:
        case GUN_LASER_AIMED_GUN:
        case GUN_ROCKET_LAUNCHER:
        case GUN_SHOUTGUN:
        case GUN_FLAME_THROWER:
        case GUN_PULSE_GUN:
            globalArgs.equipment[gun_num].gun = gun_opt;
            globalArgs.equipment[gun_num].ammo = (MAX_AMMO_FOR_GUN[gun_opt] == 0x0a00 ? 0x0b00 : MAX_AMMO_FOR_GUN[gun_opt]);
            break;
    }
} 


void kill_person(int person_id) {
    if (person_id < 0 || person_id > 4) return;

    globalArgs.alive_persons[person_id] = 0;    
}

int main(int argc, char **argv) {
    Byte aggr_buf[AGGREGATE_BUF_SIZE];
    Byte inter_buf[INTERMEDIATE_BUF_SIZE];
    Byte encrypt_buf[ENCRYPT_BUF_SIZE];
    char password[PASSWORD_LENGTH + 1] = { 0 };

    init(aggr_buf, inter_buf, encrypt_buf);


    int c;
    while (1) {
        static char *short_options = "hk:";
        static struct option long_options[] = {
            {"help",    no_argument, NULL, 'h'}, 
            {"health",  required_argument, NULL, 'b'}, 
            {"level",   required_argument, NULL, 'l'}, 
            {"gun1",    required_argument, NULL, 'o'}, 
            {"gun2",    required_argument, NULL, 'p'}, 
            {"gun3",    required_argument, NULL, 'r'}, 
            {"gun4",    required_argument, NULL, 's'}, 
            {"gun5",    required_argument, NULL, 't'}, 
            {"kill",    required_argument, NULL, 'k'}, 
            {0, 0, 0, 0},
        };

        int option_index = 0;

        c = getopt_long(argc, argv, short_options, long_options, &option_index); 

        /* Detect the end of the options. */
        if (c == -1)    break;

        switch (c) {
            case 'h':
                print_usage_message();
                exit(0);
                break;
            case 'b':
                set_health(atoi(optarg));
                break;
            case 'l':
                set_level(atoi(optarg));
                break;
            case 'o':
                set_equipment(0, atoi(optarg));
                break;
            case 'p':
                set_equipment(1, atoi(optarg));
                break;
            case 'r':
                set_equipment(2, atoi(optarg));
                break;
            case 's':
                set_equipment(3, atoi(optarg));
                break;
            case 't':
                set_equipment(4, atoi(optarg));
                break;
            case 'k':
                kill_person(atoi(optarg));
                break;
            default: 
                printf("help.. exit\n");
                exit(0);
                break;
        }
    }



    create_password(aggr_buf, inter_buf, encrypt_buf, password);
    printf("Password: [ %s ]\n", password);

    return 0;
}
