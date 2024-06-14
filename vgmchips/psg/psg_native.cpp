/*
 * This is a simple rewrite of Mitsutaka Okazaki's emu76489 library,
 * allowing this PSG emulator to be natively integrated with vgmcore.
 * In addition, this rewrite also supports a number of extra PSG
 * specifications and features in the VGM specification, including:
 *  - Output negation.
 *  - GameGear stereo disabling.
 *  - XNOR noise feedback instead of XOR.
 *  - Custom LFSR width and feedback pattern.
 * 
 * The original library can be found here:
 *   https://github.com/digital-sound-antiques/emu76489
 */

#include <psg_native.h>

/* volume table */
#define PSG_MAX_VOLUME (0.5 / 4)
static float voltbl[16] = {
    PSG_MAX_VOLUME * 1.0,
    PSG_MAX_VOLUME * 0.7943282347242815,
    PSG_MAX_VOLUME * 0.6309573444801932,
    PSG_MAX_VOLUME * 0.5011872336272722,
    PSG_MAX_VOLUME * 0.3981071705534972,
    PSG_MAX_VOLUME * 0.3162277660168379,
    PSG_MAX_VOLUME * 0.25118864315095796,
    PSG_MAX_VOLUME * 0.19952623149688792,
    PSG_MAX_VOLUME * 0.15848931924611132,
    PSG_MAX_VOLUME * 0.1258925411794167,
    PSG_MAX_VOLUME * 0.09999999999999998,
    PSG_MAX_VOLUME * 0.07943282347242814,
    PSG_MAX_VOLUME * 0.06309573444801932,
    PSG_MAX_VOLUME * 0.05011872336272722,
    PSG_MAX_VOLUME * 0.03981071705534972,
    PSG_MAX_VOLUME * 0.0
};

#define GETA_BITS 24
#define RATE 44100 // sampling rate of VGM

#define PSG_SMOOTH_OUTPUT

void psg_emu::internal_refresh() {
    if(_quality) {
        _base_incr = 1 << GETA_BITS;
        _realstep = (uint32_t) ((1ULL << 31) / RATE);
        _sngstep = (uint32_t) ((1ULL << 31) / (_clock / 16));
        _sngtime = 0;
    } else _base_incr = (uint32_t) ((double)_clock * (1 << GETA_BITS) / (16 * RATE));
}

void psg_emu::set_quality(bool q) {
    _quality = q;
    internal_refresh();
}

psg_emu::psg_emu(size_t clock, float* output_buf, pff* pan_buf, size_t lfsr_width, size_t lfsr_pattern, bool neg_output, bool no_gg_stereo, bool xnor_noise)
     : _clock(clock), _channels_out(output_buf), _channels_pan(pan_buf), _lfsr_width(lfsr_width), _lfsr_pattern(lfsr_pattern), _neg_output(neg_output), _no_gg_stereo(no_gg_stereo), _xnor_noise(xnor_noise) {
    internal_refresh();
    reset();
}
psg_emu::psg_emu(const vgm_header_t& header, float* output_buf, pff* pan_buf)
     : _clock(VGM_CLOCK(header.psg_clock)), _channels_out(output_buf), _channels_pan(pan_buf), _lfsr_width(header.psg_sr_width), _lfsr_pattern(header.psg_fb_pattern), _neg_output(header.psg_flags & PSG_NEG_OUTPUT), _no_gg_stereo(header.psg_flags & PSG_NO_GG_STEREO), _xnor_noise(header.psg_flags & PSG_XNOR_NOISE) {
    internal_refresh();
    reset();
}

void psg_emu::reset() {
    _base_count = 0;
    for(size_t i = 0; i < 3; i++) {
        _count[i] = 0;
        _freq[i] = 0;
        _edge[i] = 0;
        _volume[i] = 0x0F;
        // _mute[i] = 0;
    }
    _addr = 0;
    _noise_seed = (1 << (_lfsr_width - 1));
    _noise_count = 0;
    _noise_freq = 0;
    _noise_volume = 0x0F;
    _noise_mode = 0;
    _noise_fref = 0;

    for(size_t i = 0; i < 4; i++) {
        _channels_out[i] = 0;
        _channels_pan[i] = std::make_pair<float, float>(1, 1);
    }
}

void psg_emu::write_io(uint8_t val) {
    if(val & (1 << 7)) {
        _addr = (val & 0x70) >> 4;
        switch(_addr) {
            case 0:
            case 2:
            case 4:
                _freq[_addr >> 1] = (_freq[_addr >> 1] & 0x3F0) | (val & 0x0F);
                break;
            case 1:
            case 3:
            case 5:
                _volume[_addr >> 1] = val & 0x0F; // no need for -1 here since the shift right operation would make us lose the 1st bit anyway
                break;
            case 6:
                _noise_mode = (val & 4) >> 2;
                if((val & 0x03) == 0x03) {
                    _noise_freq = _freq[2];
                    _noise_fref = 1;
                } else {
                    _noise_freq = 32 << (val & 0x03);
                    _noise_fref = 0;
                }
                if(!_noise_freq) _noise_freq = 1;
                _noise_seed = (1 << (_lfsr_width - 1));
                break;
            case 7:
                _noise_volume = val & 0x0F;
                break;
        }
    } else _freq[_addr >> 1] = ((val & 0x3F) << 4) | (_freq[_addr >> 1] & 0x0F);
}

static inline uint32_t parity(uint32_t val) {
    val ^= val >> 16;
    val ^= val >> 8;
    val ^= val >> 4;
    val ^= val >> 2;
    val ^= val >> 1;
    return val & 1;
}

void psg_emu::update_output() {
    _base_count += _base_incr;
    uint32_t incr = (_base_count >> GETA_BITS);
    _base_count &= (1 << GETA_BITS) - 1;

    /* noise */
    _noise_count += incr;
    if(_noise_count & 0x100) {
        if(_noise_mode) {
            uint32_t p = parity(_noise_seed & _lfsr_pattern);
            if(_xnor_noise) p = 1 - p;
            _noise_seed = (_noise_seed >> 1) | (p << (_lfsr_width - 1));
        } else _noise_seed = (_noise_seed >> 1) | ((_noise_seed & 1) << (_lfsr_width - 1));
    }
    _noise_count -= (_noise_fref) ? _freq[2] : _noise_freq;
    bool sign = (_noise_seed & 1) ^ (_neg_output);
#if defined(PSG_SMOOTH_OUTPUT)
    if(sign) _channels_out[3] -= voltbl[_noise_volume]; else _channels_out[3] += voltbl[_noise_volume];
    _channels_out[3] /= 2.0;
#else
    _channels_out[3] = (sign) ? -voltbl[_noise_volume] : voltbl[_noise_volume];
#endif

    /* tone */
    for(size_t i = 0; i < 3; i++) {
        _count[i] += incr;
        if(_count[i] & 0x400) {
            if(_freq[i] > 1) {
                _edge[i] = !_edge[i];
                _count[i] -= _freq[i];
            } else _edge[i] = 1;
        }
        // if(!_mute[i])
        sign = (_edge[i]) ^ (_neg_output);
#if defined(PSG_SMOOTH_OUTPUT)
        if(sign) _channels_out[i] -= voltbl[_volume[i]]; else _channels_out[i] += voltbl[_volume[i]];
        _channels_out[i] /= 2;
#else
        _channels_out[i] = (sign) ? -voltbl[_volume[i]] : voltbl[_volume[i]];
#endif
    }
}

void psg_emu::calc() {
    if(!_quality) {
        update_output();
        return;
    }

    /* simple rate converter */
    while(_realstep > _sngtime) {
        _sngtime += _sngstep;
        update_output();
    }
    _sngtime -= _realstep;
}

void psg_emu::write_stereo(uint8_t val) {
    if(_no_gg_stereo) return; // ignore
    for(size_t ch = 0; ch < 4; ch++) {
        _channels_pan[ch].first = (val & (1 << (4 + ch))) ? 1.0 : 0.0;
        _channels_pan[ch].second = (val & (1 << ch)) ? 1.0 : 0.0;
    }
}

psg_native::psg_native(const vgm_header_t& header) : _dual_chip(header.psg_clock & VGM_CLOCK_DUAL), vgm_chip((header.psg_clock & VGM_CLOCK_DUAL) ? 8 : 4) {
    _chips[0] = new psg_emu(header, &_channels_out[0], &_channels_pan[0]);
    if(_dual_chip) _chips[1] = new psg_emu(header, &_channels_out[4], &_channels_pan[4]); // initialise 2nd chip
}

psg_native::~psg_native() {
    delete _chips[0];
    if(_dual_chip) delete _chips[1];
}

void psg_native::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return; // ignore writes to 2nd chip if it doesn't exist
    if(port) _chips[chip]->write_stereo(val);
    else _chips[chip]->write_io(val);
}

void psg_native::next_sample() {
    _chips[0]->calc();
    if(_dual_chip) _chips[1]->calc();
}

void psg_native::install(vgm_parser& parser) {
    if(!parser.header.fields.psg_clock) return; // no need to install PSG if it's not used
    if(parser.chips.psg) delete parser.chips.psg;
    parser.chips.psg = new psg_native(parser.header.fields);
}
