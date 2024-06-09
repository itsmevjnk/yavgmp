#pragma once

#include <vgm_chip.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "emu76489/emu76489.h"

/* emulator interface */
class psg_wrapper : public vgm_chip {
public:
    psg_wrapper(const vgm_header_t& header);
    ~psg_wrapper();

    virtual void next_sample();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    const bool _no_gg_stereo;
    const bool _neg_output;
    __SNG* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};