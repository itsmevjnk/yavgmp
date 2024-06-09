#pragma once

#include <vgm_chip.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "emu2413/emu2413.h"

/* emulator interface */
class opll_wrapper : public vgm_chip {
public:
    opll_wrapper(const vgm_header_t& header);
    ~opll_wrapper();

    virtual void next_sample();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    
    OPLL* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};