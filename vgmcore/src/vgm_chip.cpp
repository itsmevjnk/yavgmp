#include <vgm_chip.h>
#include <stdio.h>

vgm_chip::vgm_chip(size_t num_channels) : channels(num_channels) {
    channels_out = _channels_out = new float[num_channels];
    channels_pan = _channels_pan = new pff[num_channels];
    for(size_t i = 0; i < num_channels; i++) {
        _channels_out[i] = 0;
        _channels_pan[i] = std::make_pair<float, float>(1, 1);
    }
}

vgm_chip::~vgm_chip() {
    delete _channels_out;
    delete _channels_pan;
}

pff vgm_chip::mix_channels() {
    float left = 0, right = 0;
    for(size_t i = 0; i < channels; i++) {
        left += _channels_out[i] * _channels_pan[i].first;
        right += _channels_out[i] * _channels_pan[i].second;
    }
    return std::make_pair(left / channels, right / channels);
}