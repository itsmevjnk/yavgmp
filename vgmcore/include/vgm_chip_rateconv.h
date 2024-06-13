#pragma once

#include <vgm_chip.h>
#include <rateconv.h>

class vgm_chip_rateconv : public vgm_chip {
public:
    vgm_chip_rateconv(size_t num_channels, float f_in, bool stereo_channels = false);
    ~vgm_chip_rateconv();

    virtual void next_sample();

    virtual void clock() = 0;

    virtual void put_sample(size_t channel, float val, bool side = false);
private:
    rateconv* _rconv;
};
