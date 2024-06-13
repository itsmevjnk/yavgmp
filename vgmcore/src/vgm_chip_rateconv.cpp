#include <vgm_chip_rateconv.h>

vgm_chip_rateconv::vgm_chip_rateconv(size_t num_channels, float f_in, bool stereo_channels) : vgm_chip(num_channels, stereo_channels) {
    _rconv = new rateconv(num_channels * (stereo ? 2 : 1), f_in, 44100.0);
}

vgm_chip_rateconv::~vgm_chip_rateconv() {
    delete _rconv;
}

void vgm_chip_rateconv::put_sample(size_t channel, float val, bool side) {
    if(channel >= channels) return;
    if(!stereo) side = false;
    _rconv->put_sample(channel + (side ? channels : 0), val);

}

void vgm_chip_rateconv::next_sample() {
    int num_clocks = _rconv->advance_timer();
    for(int i = 0; i < num_clocks; i++) clock();

    for(int i = 0; i < channels; i++) _channels_out_left[i] = _rconv->get_sample(i);
    if(stereo) {
        for(int i = 0; i < channels; i++) _channels_out_right[i] = _rconv->get_sample(channels + i);
    }
}
