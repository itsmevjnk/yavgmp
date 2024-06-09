/*
 * This is a wrapper for Mitsutaka Okazaki's emu2413 library, allowing
 * it to work with vgmcore's emulator interface.
 * 
 * The original library can be found here:
 *   https://github.com/digital-sound-antiques/emu2413
 */

#include <opll_wrapper.h>

opll_wrapper::opll_wrapper(const vgm_header_t& header) : _dual_chip(header.opll_clock & VGM_CLOCK_DUAL), vgm_chip((header.opll_clock & VGM_CLOCK_DUAL) ? 28 : 14) {
    _chips[0] = OPLL_new(VGM_CLOCK(header.opll_clock), 44100); OPLL_reset(_chips[0]);
    if(_dual_chip) {
        _chips[1] = OPLL_new(VGM_CLOCK(header.opll_clock), 44100); OPLL_reset(_chips[1]);
    }
}

opll_wrapper::~opll_wrapper() {
    OPLL_delete(_chips[0]);
    if(_dual_chip) OPLL_delete(_chips[1]);
}

void opll_wrapper::update_chip(int idx) {
    OPLL_calc(_chips[idx]);
    
    /* convert channel outputs from int16_t to float */
    for(int i = 0; i < 14; i++) {
        _channels_out[idx * 14 + i] = _chips[idx]->ch_out[i] / (256.0 * (1 << 4)); // TODO: remove DC offset
    }
}

void opll_wrapper::next_sample() {
    update_chip(0);
    if(_dual_chip) update_chip(1);
}

void opll_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    OPLL_writeReg(_chips[chip], addr_reg, val);
}

void opll_wrapper::install(vgm_parser& parser) {
    if(!parser.header.fields.opll_clock) return; // no need to install OPLL if it's not used
    if(parser.chips.opll) delete parser.chips.opll;
    parser.chips.opll = new opll_wrapper(parser.header.fields);
}