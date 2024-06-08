#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>

typedef std::pair<float, float> pff; // abbreviation

class vgm_chip {
public:
    virtual void next_sample() = 0;
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) = 0;
    const std::vector<float>& channels = _channels; // channel outputs
    const std::vector<pff>& channels_pan = _channels_pan; // channel levels per output (left, right) - for mixing
    pff mix_channels(); // mix channels into stereo outputs
protected:
    std::vector<float> _channels;
    std::vector<pff> _channels_pan;
};