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
#ifdef OPN2_STEREO_CHANNELS
        true
#else
        false
#endif
    ) {
    _chips[0] = new_chip(header.opn2_clock);
    if(_dual_chip) _chips[1] = new_chip(header.opn2_clock);
    // OPN2_SetChipType((header.opn2_clock & (1 << 31)) ? 0 : ym3438_mode_ym2612); // determine if we need to enable YM2612 emulation
}

opn2_wrapper::~opn2_wrapper() {
    delete _chips[0];
    if(_dual_chip) delete _chips[1];
}

/* adapted from Wohlstand's OPN2_Generate function */
void opn2_wrapper::update_chip(int idx) {
    uint32_t mute = 0;
    int channel = -1;

    ym3438_t* chip = _chips[idx];
#ifdef OPN2_STEREO_CHANNELS
    float samples[6][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
#else
    float samples[6] = {0, 0, 0, 0, 0, 0};
#endif
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
#ifdef OPN2_STEREO_CHANNELS
                samples[channel][0] += buf[0] / 256.0; // ym3438.h says MOL and MOR are 9-bit signed values
                samples[channel][1] += buf[1] / 256.0;
#else
                samples[channel] += (buf[0] + buf[1]) / (2 * 256.0);
#endif
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
#ifdef OPN2_STEREO_CHANNELS
            samples[ch][0] /= sample_cnts[ch];
            samples[ch][1] /= sample_cnts[ch];
#else
            samples[ch] /= sample_cnts[ch];
#endif
        }
#ifdef OPN2_STEREO_CHANNELS
        put_sample(ch, samples[ch][0], false);
        put_sample(ch, samples[ch][1], true);
#else
        put_sample(ch, samples[ch]);
#endif
    }
}

void opn2_wrapper::clock() {
    update_chip(0);
    if(_dual_chip) update_chip(1);
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