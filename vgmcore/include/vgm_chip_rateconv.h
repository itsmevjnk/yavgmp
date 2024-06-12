#pragma once

#include <vgm_chip.h>

#define RATECONV_LW                             16
#define RATECONV_SINC_RESO                      256

class vgm_chip_rateconv : public vgm_chip {
public:
    vgm_chip_rateconv(size_t num_channels, float f_in, bool stereo_channels = false);
    ~vgm_chip_rateconv();

    virtual void next_sample();

    virtual void clock() = 0;

    virtual void put_sample(size_t channel, float val, bool side = false);
    void reset_rateconv();
private:
    const bool _stereo;
    const float _f_ratio;
    float* _buf;
    float _timer = 0;

    float _sinctab[RATECONV_SINC_RESO * RATECONV_LW / 2];
};
