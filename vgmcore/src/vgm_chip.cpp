#include <vgm_chip.h>
#include <stdio.h>

#include <stdexcept>

vgm_chip::vgm_chip(size_t num_channels, bool stereo_channels) : channels(num_channels), stereo(stereo_channels) {
    if(stereo_channels) {
        channels_out_left = _channels_out_left = new float[num_channels];
        channels_out_right = _channels_out_right = new float[num_channels];
    } else channels_out_left = channels_out_right = _channels_out = _channels_out_left = _channels_out_right = new float[num_channels];
    channels_pan = _channels_pan = new pff[num_channels];
    for(size_t i = 0; i < num_channels; i++) {
        _channels_out_left[i] = _channels_out_right[i] = 0;
        _channels_pan[i] = std::make_pair<float, float>(1, 1);
    }
}

vgm_chip::~vgm_chip() {
    if(_channels_out) delete _channels_out;
    else {
        delete _channels_out_left;
        delete _channels_out_right;
    }
    delete _channels_pan;
}

pff vgm_chip::mix_channels() {
    float left = 0, right = 0;
    for(size_t i = 0; i < channels; i++) {
        left += _channels_out_left[i] * _channels_pan[i].first;
        right += _channels_out_right[i] * _channels_pan[i].second;
    }
    return std::make_pair(left / channels, right / channels);
}

/* many chips won't have this method implemented, so it's safer to provide a generic implementation that throws an error */

void vgm_chip::write_rom(uintptr_t chip, size_t total_size, uintptr_t base_addr, const uint8_t* data, size_t len, size_t type) {
    throw std::runtime_error("write_rom implementation not provided");
}

void vgm_chip::write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len) {
    throw std::runtime_error("write_ram implementation not provided");
}