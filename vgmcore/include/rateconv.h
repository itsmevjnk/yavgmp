#pragma once

#include <vgm_types.h>

#define RATECONV_LW_MIN                         16
#define RATECONV_SINC_RESO                      256

class rateconv {
public:
    rateconv(int num_channels, float f_in, float f_out);
    ~rateconv();

    void put_sample(int channel, sample_t val);

    int advance_timer(); // must be called prior to calling get_sample() for each channel - returns the number of input samples to be added per channel
    sample_t get_sample(int channel);

    const int channels;
private:
    const float _f_ratio;
    float _timer;
    int _lw;

    sample_t* _buf;
    float* _sinctab;
};