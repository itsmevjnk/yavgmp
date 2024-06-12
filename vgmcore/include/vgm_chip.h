#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef std::pair<float, float> pff; // abbreviation

class vgm_chip {
public:
    vgm_chip(size_t num_channels, bool stereo_channels = false);
    ~vgm_chip();

    virtual void next_sample() = 0;
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) = 0;
    virtual void write_rom(uintptr_t chip, size_t total_size, uintptr_t base_addr, const uint8_t* data, size_t len, size_t type = 0);
    virtual void write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len);
    const size_t channels; // number of channels
    const float* channels_out_left, *channels_out_right; // channel outputs
    const pff* channels_pan; // channel levels per output (left, right) - for mixing
    const bool stereo; // set if channels are stereo
    virtual pff mix_channels(); // mix channels into stereo outputs (NOTE: chips can provide their own implementations too - eg. OPN2 with its TDM mixing)
protected:
    float* _channels_out = nullptr; // only set if stereo_channels = false
    float* _channels_out_left = nullptr, *_channels_out_right = nullptr; // set to _channels_out if stereo_channels = false; their own buffers otherwise (and _channels_out would be null)
    pff* _channels_pan;
};