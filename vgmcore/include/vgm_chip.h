#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef std::pair<float, float> pff; // abbreviation

class vgm_chip {
public:
    vgm_chip(size_t num_channels);
    ~vgm_chip();

    virtual void next_sample() = 0;
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) = 0;
    virtual void write_rom(uintptr_t chip, size_t total_size, uintptr_t base_addr, const uint8_t* data, size_t len, size_t type = 0);
    virtual void write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len);
    const size_t channels; // number of channels
    const float* channels_out = _channels_out; // channel outputs
    const pff* channels_pan = _channels_pan; // channel levels per output (left, right) - for mixing
    pff mix_channels(); // mix channels into stereo outputs
protected:
    float* _channels_out;
    pff* _channels_pan;
};