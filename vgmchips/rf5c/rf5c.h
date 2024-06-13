/*
 * This code is largely based on TritonCore's RF5C68 emulator,
 * available via these links:
 *   https://github.com/michelgerritse/TritonCore/blob/master/Devices/Sound/RF5C68.cpp
 *   https://github.com/michelgerritse/TritonCore/blob/master/Devices/Sound/RF5C68.h
 * 
 * Ported code <C> 2024 Thanh Vinh Nguyen.
 * The ported source code is available under the MIT license.
 * See the LICENSE file in this source tree's root directory.
 * 
 * Original code <C> 2023 Michel Gerritse. All rights reserved.
 * The original source code is available under the BSD-3-Clause
 * license.
 * See the LICENSE-BSD.txt file in this source file's directory.
 */

#pragma once

#include <vgm_chip_rateconv.h>
#include <vgm_header.h>
#include <vgm_parser.h>

typedef struct {
    bool on;
    uint8_t env;
    // uint8_t pan_l;
    // uint8_t pan_r;
    uint16_t freq_delta;
    uint16_t loop_start;
    uint16_t start_addr;

    /* internal state */
    uint32_t addr;
    // uint32_t pmul_l; // env * pan_l
    // uint32_t pmul_r; // env * pan_r
} rf5c_channel_t;

class rf5c : public vgm_chip_rateconv {
public:
    rf5c(uint32_t clock, bool rf164, bool ramax = false); // NOTE: enabling RAMAX messes with Sega CD V2.xx US BIOS music!
    ~rf5c();

    virtual void clock();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    virtual void write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len);
    
    static void install(vgm_parser& parser, bool rf68 = true, bool rf164 = true); // create and plant ourselves in the specified parser object
private:
    void update_chip(int idx);
    void write_reg(uint32_t addr, uint8_t data);

    const uint32_t _shift; // fixed point shift (16.11 or 17.10)
    const uint16_t _out_mask; // DAC output mask

    uint32_t _cycles_to_do = 0;

    bool _sounding = false;
    uint32_t _channel_bank = 0;
    uint32_t _wave_bank = 0;
    uint8_t _channel_ctrl = 0xFF;

    rf5c_channel_t _channels[8];

    uint8_t* ram;
    const size_t ram_size;
};
