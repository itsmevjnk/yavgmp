#include <emu2149_wrapper.h>

static inline PSG* new_chip(uint32_t clock, bool ckdiv, bool ym) {
#ifdef EMU2149_USE_BUILTIN_RATECONV
    PSG* chip = PSG_new(clock, 44100);
#else
    PSG* chip = PSG_new(clock, clock / 8);
#endif
    if(ckdiv) PSG_setClockDivider(chip, 1);
    PSG_setVolumeMode(chip, (ym) ? 1 : 2); // set volume mode between YM2149F and AY-3-8910
#ifndef EMU2149_USE_BUILTIN_RATECONV
    PSG_setQuality(chip, 0); // bypass internal rate converter
#endif
    PSG_reset(chip);
    return chip;
}

emu2149_wrapper::emu2149_wrapper(const vgm_header_t& header) : _dual_chip(header.ay8910_clock & VGM_CLOCK_DUAL),
#ifdef EMU2149_USE_BUILTIN_RATECONV
    vgm_chip((header.ay8910_clock & VGM_CLOCK_DUAL) ? 6 : 3) {
#else
    vgm_chip_rateconv((header.ay8910_clock & VGM_CLOCK_DUAL) ? 6 : 3, VGM_CLOCK(header.ay8910_clock) / 8) {
#endif
    uint32_t clock = VGM_CLOCK(header.ay8910_clock);
    bool ym = (header.ay8910_type & 0x10);
    bool ckdiv = ym && (header.ay8910_flags & AY8910_YM_PIN26_LOW);
    _chips[0] = new_chip(clock, ckdiv, ym);
    if(_dual_chip) _chips[1] = new_chip(clock, ckdiv, ym);
}

emu2149_wrapper::~emu2149_wrapper() {
    PSG_delete(_chips[0]);
    if(_dual_chip) PSG_delete(_chips[1]);
}

void emu2149_wrapper::update_chip(int idx) {
    PSG_calc(_chips[idx]);
    for(int i = 0; i < 3; i++)
#ifdef EMU2149_USE_BUILTIN_RATECONV
        _channels_out[idx * 3 + i] = (sample_t)_chips[idx]->ch_out[i] / (0xFF << 4);
#else
        put_sample(idx * 3 + i, (sample_t)_chips[idx]->ch_out[i] / (0xFF << 4) - 0.5);
#endif
}

#ifdef EMU2149_USE_BUILTIN_RATECONV
void emu2149_wrapper::next_sample() {
#else
void emu2149_wrapper::clock() {
#endif
    update_chip(0);
    if(_dual_chip) update_chip(1);
}

void emu2149_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    if(port) {
        /* write stereo mask */
        int base = chip * 3;
        for(int i = 0; i < 3; i++) {
            _channels_pan[base + i].left = (val & (1 << (i << 1))) ? 1.0 : 0.0; // left
            _channels_pan[base + i].right = (val & (1 << ((i << 1) + 1))) ? 1.0 : 0.0; // right
        }
    } else {
        /* normal I/O write */
        PSG_writeReg(_chips[chip], addr_reg, val);
    }
}

void emu2149_wrapper::install(vgm_parser& parser) {
    if(!parser.header.fields.ay8910_clock) return; // no need to install if it's not used
    if(parser.chips.ay8910) delete parser.chips.ay8910;
    parser.chips.ay8910 = new emu2149_wrapper(parser.header.fields);
}