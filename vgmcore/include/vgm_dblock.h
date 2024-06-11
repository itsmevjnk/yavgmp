#pragma once

#include <stddef.h>
#include <stdint.h>

enum vgm_comp_type {
    COMP_BITPACK = 0x00,
    COMP_DPCM = 0x01
};

enum vgm_comp_subtype {
    BP_COPY = 0x00,
    BP_SHL = 0x01,
    BP_TABLE = 0x02
};

#define VGM_DBLOCK_HEADER_SIZE                          (sizeof(uint8_t) + sizeof(uint32_t)) // data block header size (.type + .size)

/* data block structure */
#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint32_t size;
    union {
        struct {
            uint8_t type; // see vgm_comp_type enum
            uint32_t uncomp_size;
            uint8_t bd; // bits decompressed
            uint8_t bc; // bits compressed
            uint8_t subtype; // see vgm_comp_subtype enum; only applicable for bit-packing compression for now
            uint16_t off; // offset value (to be added)
            uint8_t data[0];
        } compressed; // type = 0x40..7E

        struct {
            uint8_t type;
            uint8_t subtype;
            uint8_t bd;
            uint8_t bc;
            uint16_t values;
            uint8_t data[0];
        } decomp_table; // type = 0x7F

        struct {
            uint32_t full_size; // size of the entire ROM
            uint32_t base; // base address of our block
            uint8_t data[0];
        } dump; // type = 0x80..BF

        struct {
            uint16_t base;
            uint8_t data[0];
        } write_16; // type = 0xC0..DF
        
        struct {
            uint32_t base;
            uint8_t data[0];
        } write_32; // type = 0xE0..FF
        uint8_t uncompressed[0]; // type == 0x00..3F
    } data;
} vgm_dblock_t;
#pragma pack(pop)