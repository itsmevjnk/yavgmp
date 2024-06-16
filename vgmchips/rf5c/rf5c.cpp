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

#include <rf5c.h>
#include <string.h>

void rf5c::write_reg(uint32_t addr, uint8_t data) {
    addr &= 0x1FFF;

    if(addr & 0x1000) ram[_wave_bank | (addr & 0xFFF)] = data;
    else {
        rf5c_channel_t& channel = _channels[_channel_bank];

        switch(addr) {
            case 0x00: // envelope reg
                channel.env = data;
                // printf("ch%u env=0x%x\n", _channel_bank, channel.env);
                // channel.pmul_l = channel.env * channel.pan_l;
                // channel.pmul_r = channel.env * channel.pan_r;
                break;
            case 0x01: // pan reg (passed to vgm_chip panning)
                // channel.pan_r = data >> 4; channel.pan_l = data & 0xF;
                // channel.pmul_l = channel.env * channel.pan_l;
                // channel.pmul_r = channel.env * channel.pan_r;
                // printf("ch%u pan=0x%x\n", _channel_bank, data);
                _channels_pan[_channel_bank].left = (data & 0xF) / 15.0;
                _channels_pan[_channel_bank].right = (data >> 4) / 15.0;
                break;
            case 0x02: // freq delta (LSB)
                channel.freq_delta = (channel.freq_delta & 0xFF00) | data;
                break;
            case 0x03: // freq delta (MSB)
                channel.freq_delta = (channel.freq_delta & 0x00FF) | (data << 8);
                break;
            case 0x04: // loop start (LSB)
                channel.loop_start = (channel.loop_start & 0xFF00) | data;
                break;
            case 0x05: // loop start (MSB)
                channel.loop_start = (channel.loop_start & 0x00FF) | (data << 8);
                break;
            case 0x06: // start address (MSB)
                channel.start_addr = data << 8;
                break;
            case 0x07: // control reg
                _sounding = data >> 7;
                if(data & 0x40) _channel_bank = data & 0x07;
                else _wave_bank = (data & 0x0F) << 12;
                break;
            case 0x08: // channel on/off reg
                _channel_ctrl = ~data; // 0 = on, 1 = off
                for(int i = 0; i < 8; i++) {
                    bool on = ((_channel_ctrl >> i) & 1);
                    if((on ^ _channels[i].on) && on) {
                        /* restarting channel, load start address */
                        _channels[i].addr = _channels[i].start_addr << _shift;
                    }
                    _channels[i].on = on;
                }
                break;
            default:
                break;
        }
    }
}

rf5c::rf5c(uint32_t clock, bool rf164, bool ramax) : _out_mask(rf164 ? ~0x00 : ~0x3F), ram_size(((rf164 && ramax) ? 128 : 64) * 1024), _shift((rf164 && ramax) ? 10 : 11), vgm_chip_rateconv(8, clock / 384.0) {
    ram = new uint8_t[ram_size];

    /* reset */
    memset(_channels, 0, sizeof(_channels));
    memset(_channels_out, 0, sizeof(_channels_out));
    memset(ram, 0, ram_size * sizeof(uint8_t));
    for(int i = 0; i < 8; i++) _channels_pan[i].left = _channels_pan[i].right = 0; // mute all channels first
}

rf5c::~rf5c() {
    delete ram;
}

void rf5c::clock() {
    for(int i = 0; i < 8; i++) {
        rf5c_channel_t& channel = _channels[i];

        if(_sounding && channel.on) {
            uint8_t pcm = ram[channel.addr >> _shift];
            if(pcm == 0xFF) {
                /* loop stop data */
                channel.addr = channel.loop_start << _shift;
                pcm = ram[channel.addr >> _shift];
                if(pcm == 0xFF) continue;
            }

            channel.addr = (channel.addr + channel.freq_delta) & 0x07FFFFFF; // advance addr counter

            /* apply envelope and write to buffer */
            if(pcm & 0x80) {
                pcm &= 0x7F;
                put_sample(i, (pcm * channel.env) / 32768.0);
            } else {
                put_sample(i,  -(pcm * channel.env) / 32768.0);
            }
        } else put_sample(i, 0.0);
    }
}

void rf5c::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(port) {
        /* memory write */
        if(addr_reg >= ram_size) return; // ignore out-of-bound write
        ram[addr_reg] = val;
    } else write_reg(addr_reg, val); // register write
}

void rf5c::write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len) {
    base_addr += _wave_bank;
    if(base_addr >= ram_size) return;
    if(base_addr + len > ram_size) len = ram_size - base_addr;

    memcpy(&ram[base_addr], data, len);
}

void rf5c::install(vgm_parser& parser, bool rf68, bool rf164) {
    if(rf68 && parser.header.fields.rf68_clock) {
        if(parser.chips.rf68) delete parser.chips.rf68;
        parser.chips.rf68 = new rf5c(VGM_CLOCK(parser.header.fields.rf68_clock), false);
    }

    if(rf164 && parser.header.fields.rf164_clock) {
        if(parser.chips.rf164) delete parser.chips.rf164;
        parser.chips.rf164 = new rf5c(VGM_CLOCK(parser.header.fields.rf164_clock), true);
    }
}
