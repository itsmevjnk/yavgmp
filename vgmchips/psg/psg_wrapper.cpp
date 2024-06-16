/*
 * This is a wrapper for Mitsutaka Okazaki's emu76489 library, allowing
 * it to work with vgmcore's emulator interface.
 * 
 * The original library can be found here:
 *   https://github.com/digital-sound-antiques/emu76489
 */

#include <psg_wrapper.h>

psg_wrapper::psg_wrapper(const vgm_header_t& header) : _dual_chip(header.psg_clock & VGM_CLOCK_DUAL), _no_gg_stereo(header.psg_flags & PSG_NO_GG_STEREO), _neg_output(header.psg_flags & PSG_NEG_OUTPUT), vgm_chip((header.psg_clock & VGM_CLOCK_DUAL) ? 8 : 4) {
    _chips[0] = SNG_new(VGM_CLOCK(header.psg_clock), 44100); SNG_set_quality(_chips[0], 1); SNG_reset(_chips[0]);
    if(_dual_chip) {
        _chips[1] = SNG_new(VGM_CLOCK(header.psg_clock), 44100);
        SNG_set_quality(_chips[1], 1);
        SNG_reset(_chips[1]);
    }
}

psg_wrapper::~psg_wrapper() {
    SNG_delete(_chips[0]);
    if(_dual_chip) SNG_delete(_chips[1]);
}

#define PSG_MAX_VOLUME (1.0 / 4)
void psg_wrapper::update_chip(int idx) {
    SNG_calc(_chips[idx]);
    
    /* convert channel outputs from int16_t to float */
    for(int i = 0; i < 4; i++) {
        _channels_out[idx * 4 + i] = _chips[idx]->ch_out[i] / (256.0 * (1 << 4) * PSG_MAX_VOLUME); // TODO: remove DC offset
        if(_neg_output) _channels_out[idx * 4 + i] = 1.0 - _channels_out[idx * 4 + i];
    }
}

void psg_wrapper::next_sample() {
    update_chip(0);
    if(_dual_chip) update_chip(1);
}

void psg_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    if(!port) SNG_writeIO(_chips[chip], val);
    else if(!_no_gg_stereo) {
        /* intercept GG write to use our own handler */
        int base_ch = (chip) ? 4 : 0;
        for(size_t ch = 0; ch < 4; ch++) {
            _channels_pan[base_ch + ch].left = (val & (1 << (4 + ch))) ? 1.0 : 0.0;
            _channels_pan[base_ch + ch].right = (val & (1 << ch)) ? 1.0 : 0.0;
        }
    }
}

void psg_wrapper::install(vgm_parser& parser) {
    if(!parser.header.fields.psg_clock) return; // no need to install PSG if it's not used
    if(parser.chips.psg) delete parser.chips.psg;
    parser.chips.psg = new psg_wrapper(parser.header.fields);
}