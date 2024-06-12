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
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    
    ym3438_t* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};