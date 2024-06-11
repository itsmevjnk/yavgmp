#pragma once

#include <vgm_chip.h>

#define RATECONV_LW                             16
#define RATECONV_SINC_RESO                      256

class vgm_chip_rateconv : public vgm_chip {
public:
    vgm_chip_rateconv(size_t num_channels, size_t f_in);
    ~vgm_chip_rateconv();

    virtual void next_sample();

    virtual void clock() = 0;

    virtual void put_sample(size_t channel, float val);
    void reset_rateconv();
private:
    const float _f_ratio;
    float* _buf;
    float _timer = 0;

    float _sinctab[RATECONV_SINC_RESO * RATECONV_LW / 2];
};
