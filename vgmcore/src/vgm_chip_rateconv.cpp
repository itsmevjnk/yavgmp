#include <vgm_chip_rateconv.h>
#include <string.h>
#include <cmath>

vgm_chip_rateconv::vgm_chip_rateconv(size_t num_channels, float f_in, bool stereo_channels) : _stereo(stereo_channels), _rate_ratio(44100.0 / f_in), vgm_chip(num_channels, stereo_channels) {
    _samples_left = new float[num_channels];
    _old_samples_left = new float[num_channels];
    if(stereo_channels) {
        _samples_right = new float[num_channels];
        _old_samples_right = new float[num_channels];
    }
}

vgm_chip_rateconv::~vgm_chip_rateconv() {
    delete _samples_left; delete _old_samples_left;
    if(_stereo) {
        delete _samples_right; delete _old_samples_right;
    }
}

void vgm_chip_rateconv::put_sample(size_t channel, float val, bool side) {
    if(channel >= channels) return;
    if(!_stereo) side = false;
    if(side) {
        _old_samples_right[channel] = _samples_right[channel];
        _samples_right[channel] = val;
    } else {
        _old_samples_left[channel] = _samples_left[channel];
        _samples_left[channel] = val;
    }
}

void vgm_chip_rateconv::next_sample() {
    while(_sample_cnt >= _rate_ratio) {
        clock();
        _sample_cnt -= _rate_ratio;
    }

    for(int i = 0; i < channels; i++) {
        _channels_out_left[i] = (_old_samples_left[i] * (_rate_ratio - _sample_cnt) + _samples_left[i] * _sample_cnt) / _rate_ratio;
        if(_stereo) _channels_out_right[i] = (_old_samples_right[i] * (_rate_ratio - _sample_cnt) + _samples_right[i] * _sample_cnt) / _rate_ratio;
    }

    _sample_cnt++;
}
