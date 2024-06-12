#pragma once

#include <vgm_chip.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "emu8950/emu8950.h"

enum opl_types {
    OPL_Y8950 = 0,
    OPL_YM3526 = 1,
    OPL_YM3812 = 2
};

/* emulator interface */
class opl_wrapper : public vgm_chip {
public:
    opl_wrapper(uint32_t clock, enum opl_types type);
    ~opl_wrapper();

    virtual void next_sample();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser, bool opl = true, bool opl2 = true, bool msx = true); // create and plant ourselves in the specified parser object (the boolean params specify whether to install for OPL (YM3526), OPL2 (YM3812) or MSX-Audio (Y8950))
private:
    const bool _dual_chip;
    
    OPL* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};