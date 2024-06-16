#include <opn2_wrapper.h>

static inline ym3438_t* new_chip(uint32_t clock) {
    ym3438_t* chip = new ym3438_t;
    OPN2_Reset(chip, 44100, VGM_CLOCK(clock));
    return chip;
}

opn2_wrapper::opn2_wrapper(const vgm_header_t& header) : _dual_chip(header.opn2_clock & VGM_CLOCK_DUAL),
    vgm_chip_rateconv(
        (header.opn2_clock & VGM_CLOCK_DUAL) ? 12 : 6,
        VGM_CLOCK(header.opn2_clock) / (float)(6 * 24), // 6 master clocks per internal clock * 24 internal clocks per cycle run
        true
    ) {
    _mix_rconv = new rateconv((header.opn2_clock & VGM_CLOCK_DUAL) ? 4 : 2, VGM_CLOCK(header.opn2_clock) / 6.0, 44100.0); // we'll have data to feed into this on every internal clock cycle
    _chips[0] = new_chip(header.opn2_clock);
    if(_dual_chip) _chips[1] = new_chip(header.opn2_clock);
    // OPN2_SetChipType((header.opn2_clock & (1 << 31)) ? 0 : ym3438_mode_ym2612); // determine if we need to enable YM2612 emulation
}

opn2_wrapper::~opn2_wrapper() {
    delete _chips[0];
    if(_dual_chip) delete _chips[1];
}

#define OPN2_MIX_AMP                            2.5 // amplification factor for mixed outputs (to counteract volume drop due to resampling)

/* adapted from Wohlstand's OPN2_Generate function */
void opn2_wrapper::update_chip(int idx) {
    uint32_t mute = 0;
    int channel = -1;

    ym3438_t* chip = _chips[idx];
    sample_t samples[6][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    int sample_cnts[6] = {0, 0, 0, 0, 0, 0};
    for(int i = 0; i < 24; i++) {
        switch(chip->cycles >> 2) {
            case 0: channel = 1; mute = chip->mute[1]; break;
            case 1: channel = 5; mute = chip->mute[5 + chip->dacen]; break;
            case 2: channel = 3; mute = chip->mute[3]; break;
            case 3: channel = 0; mute = chip->mute[0]; break;
            case 4: channel = 4; mute = chip->mute[4]; break;
            case 5: channel = 2; mute = chip->mute[2]; break;
            default: break;
        }
        int16_t buf[2];
        OPN2_Clock(chip, buf);
        
        if(channel >= 0) {
            if(!mute) {
                sample_t left = buf[0] / 256.0, right = buf[1] / 256.0; // ym3438.h says MOL and MOR are 9-bit signed values
                samples[channel][0] += left;
                samples[channel][1] += right;
                _mix_rconv->put_sample((idx << 1), left * _channels_pan[(idx * 6) + channel].left);
                _mix_rconv->put_sample((idx << 1) | 1, right * _channels_pan[(idx * 6) + channel].right);
            } else {
                _mix_rconv->put_sample((idx << 1), 0);
                _mix_rconv->put_sample((idx << 1) | 1, 0);
            }
            sample_cnts[channel]++;
        }

        while(chip->writebuf[chip->writebuf_cur].time <= chip->writebuf_samplecnt) {
            if(!(chip->writebuf[chip->writebuf_cur].port & 0x04)) break;
            chip->writebuf[chip->writebuf_cur].port &= 0x03;
            OPN2_Write(chip, chip->writebuf[chip->writebuf_cur].port, chip->writebuf[chip->writebuf_cur].data);
            chip->writebuf_cur = (chip->writebuf_cur + 1) % OPN_WRITEBUF_SIZE;
        }
        chip->writebuf_samplecnt++;
    }

    /* add sample to rate converter */
    for(int ch = 0; ch < 6; ch++) {
        if(sample_cnts[ch]) {
            samples[ch][0] /= sample_cnts[ch];
            samples[ch][1] /= sample_cnts[ch];
        }
        put_sample(ch, samples[ch][0], false);
        put_sample(ch, samples[ch][1], true);
    }
}

void opn2_wrapper::clock() {
    update_chip(0);
    if(_dual_chip) update_chip(1);

    /* calculate mixed output */
    _mix_rconv->advance_timer();
    _mix_output.left = _mix_rconv->get_sample(0);
    _mix_output.right = _mix_rconv->get_sample(1);
    if(_dual_chip) {
        _mix_output.left = (_mix_output.left + _mix_rconv->get_sample(2)) / 2;
        _mix_output.right = (_mix_output.right + _mix_rconv->get_sample(3)) / 2;
    }
    _mix_output.left *= OPN2_MIX_AMP; _mix_output.right *= OPN2_MIX_AMP;
}

void opn2_wrapper::write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {
    if(!_dual_chip && chip) return;

    OPN2_WriteBuffered(_chips[chip], (port << 1) | 0, addr_reg);
    OPN2_WriteBuffered(_chips[chip], (port << 1) | 1, val);
}

void opn2_wrapper::install(vgm_parser& parser) {
    if(!parser.header.fields.opn2_clock) return; // no need to install if it's not used
    if(parser.chips.opn2) delete parser.chips.opn2;
    parser.chips.opn2 = new opn2_wrapper(parser.header.fields);
}

stereo_sample_t opn2_wrapper::mix_channels() {
    return _mix_output;
}