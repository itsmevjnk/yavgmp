#pragma once

#define RATECONV_LW_MIN                         16
#define RATECONV_SINC_RESO                      256

class rateconv {
public:
    rateconv(int num_channels, float f_in, float f_out);
    ~rateconv();

    void put_sample(int channel, float val);

    int advance_timer(); // must be called prior to calling get_sample() for each channel - returns the number of input samples to be added per channel
    float get_sample(int channel);

    const int channels;
private:
    const float _f_ratio;
    float _timer;
    int _lw;

    float* _buf;
    float* _sinctab;
};