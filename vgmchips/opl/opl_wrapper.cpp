#include <opl_wrapper.h>

opl_wrapper::opl_wrapper(uint32_t clock, enum opl_types type) : _dual_chip(clock & VGM_CLOCK_DUAL), vgm_chip((clock & VGM_CLOCK_DUAL) ? 30 : 15) {
    _chips[0] = OPL_new(VGM_CLOCK(clock), 44100); OPL_reset(_chips[0]);
    if(_dual_chip) {
        _chips[1] = OPL_new(VGM_CLOCK(clock), 44100); OPL_reset(_chips[1]);
    }
}

opl_wrapper::~opl_wrapper() {
    OPL_delete(_chips[0]);
    if(_dual_chip) OPL_delete(_chips[1]);
}

void opl_wrapper::update_chip(int idx) {
    OPL_calc(_chips[idx]);
    
    /* convert channel outputs from int16_t to float */
    for(int i = 0; i < 15; i++) {
        _channels_out[idx * 15 + i] = _chips[idx]->ch_out[i] / (256.0 * (1 << 4)); // TODO: remove DC offset
    }
}

void opl_wrapper::next_sample() {
    update_chip(0);
    if(_dual_chip) update_chip(1);
}

void opl_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    OPL_writeReg(_chips[chip], addr_reg, val);
}

void opl_wrapper::install(vgm_parser& parser, bool opl, bool opl2, bool msx) {
    if(opl && parser.header.fields.opl_clock) {
        if(parser.chips.opl) delete parser.chips.opl;
        parser.chips.opl = new opl_wrapper(parser.header.fields.opl_clock, OPL_YM3526);
    }
    if(opl2 && parser.header.fields.opl2_clock) {
        if(parser.chips.opl2) delete parser.chips.opl2;
        parser.chips.opl2 = new opl_wrapper(parser.header.fields.opl2_clock, OPL_YM3812);
    }
    if(msx && parser.header.fields.msx_clock) {
        if(parser.chips.msx) delete parser.chips.msx;
        parser.chips.msx = new opl_wrapper(parser.header.fields.msx_clock, OPL_Y8950);
    }
}