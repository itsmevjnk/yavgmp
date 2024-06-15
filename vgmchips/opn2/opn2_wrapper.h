#pragma once

#include <vgm_chip_rateconv.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "nukedopn2/ym3438.h"

class opn2_wrapper : public vgm_chip_rateconv {
public:
    opn2_wrapper(const vgm_header_t& header);
    ~opn2_wrapper();

    virtual void clock();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    virtual stereo_sample_t mix_channels();
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    rateconv* _mix_rconv; // for mixing channels (since OPN2 uses time-division multiplexing)
    stereo_sample_t _mix_output; // mixed output (written by clock(), to be returned by mix_channels())
    
    ym3438_t* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};