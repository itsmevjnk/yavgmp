#pragma once

#include <vgm_chip.h>
#include <vgm_header.h>
#include <vgm_parser.h>
#include "emu2212/emu2212.h"

/* emulator interface */
class scc_wrapper : public vgm_chip {
public:
    scc_wrapper(const vgm_header_t& header);
    ~scc_wrapper();

    virtual void next_sample();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    
    SCC* _chips[2] = {nullptr, nullptr}; // chips

    void update_chip(int idx);
};