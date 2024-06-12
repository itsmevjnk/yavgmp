#include <scc_wrapper.h>

scc_wrapper::scc_wrapper(const vgm_header_t& header) : _dual_chip(header.scc_clock & VGM_CLOCK_DUAL), vgm_chip((header.scc_clock & VGM_CLOCK_DUAL) ? 10 : 5) {
    uint8_t is_plus = header.scc_clock >> 31;
    _chips[0] = SCC_new(VGM_CLOCK(header.scc_clock), 44100); SCC_reset(_chips[0]); SCC_writeReg(_chips[0], 0xE0, is_plus); 
    if(_dual_chip) {
        _chips[1] = SCC_new(VGM_CLOCK(header.scc_clock), 44100); SCC_reset(_chips[1]); SCC_writeReg(_chips[1], 0xE0, is_plus);
    }
}

scc_wrapper::~scc_wrapper() {
    SCC_delete(_chips[0]);
    if(_dual_chip) SCC_delete(_chips[1]);
}

void scc_wrapper::update_chip(int idx) {
    SCC_calc(_chips[idx]);
    
    /* convert channel outputs from int16_t to float */
    for(int i = 0; i < 5; i++) {
        _channels_out[idx * 5 + i] = _chips[idx]->ch_out[i] / (256.0 * (1 << 4)); // TODO: remove DC offset
    }
}

void scc_wrapper::next_sample() {
    update_chip(0);
    if(_dual_chip) update_chip(1);
}

void scc_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    switch(port) {
        case 0x00: // wavetable write
            SCC_writeReg(_chips[chip], addr_reg, val);
            break;
        case 0x01: // set frequency
            SCC_writeReg(_chips[chip], 0xC0 + addr_reg, val);
            break;
        case 0x02: // set volume
            SCC_writeReg(_chips[chip], 0xD0 + addr_reg, val);
            break;
        case 0x03: // set mask
            SCC_setMask(_chips[chip], ~val & 0x1F);
            break;
        default: break;
    }
}

void scc_wrapper::install(vgm_parser& parser) {
    if(!parser.header.fields.scc_clock) return; // no need to install OPLL if it's not used
    if(parser.chips.scc) delete parser.chips.scc;
    parser.chips.scc = new scc_wrapper(parser.header.fields);
}