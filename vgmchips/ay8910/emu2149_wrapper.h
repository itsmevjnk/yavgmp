#pragma once

#include <vgm_chip_rateconv.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "emu2149/emu2149.h"

#ifdef EMU2149_USE_BUILTIN_RATECONV
class emu2149_wrapper : public vgm_chip {
#else
class emu2149_wrapper : public vgm_chip_rateconv {
#endif
public:
    emu2149_wrapper(const vgm_header_t& header);
    ~emu2149_wrapper();

#ifdef EMU2149_USE_BUILTIN_RATECONV
    virtual void next_sample();
#else
    virtual void clock();
#endif
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    
    PSG* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};