#include <vgm_parser.h>

void vgm_parser::do_ay8910_stereo_mask(uint8_t cmd) {
    uint8_t chip = (cmd & 0x80) >> 7;
    uint8_t mask = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x\tAY8910#%u\tstereo mask 0x%02x\n", cmd, mask, chip, mask);
    if(chips.ay8910) chips.ay8910->write(chip, 1, 0, mask);
} // 0x31

void vgm_parser::do_mikey_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tMikey\treg 0x%02x <- 0x%02x", cmd, reg, data, reg, data);
    if(chips.mikey) chips.mikey->write(0, 0, reg, data); // Mikey emulator can discard chip and port
} // 0x40

void vgm_parser::do_psg_stereo_write(uint8_t cmd) {
    uint8_t chip = (cmd == 0x3F) ? 1 : 0;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x\tPSG#%u\tstereo mask 0x%02x\n", cmd, data, chip + 1, data);
    if(chips.psg) chips.psg->write(chip, 1, 0x06, data);
} // 0x4F

void vgm_parser::do_psg_write(uint8_t cmd) {
    uint8_t chip = (cmd == 0x30) ? 1 : 0;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x\tPSG#%u\twrite 0x%02x\n", cmd, data, chip + 1, data);
    if(chips.psg) chips.psg->write(chip, 0, 0, data);
} // 0x50

void vgm_parser::do_opll_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPLL#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.opll) chips.opll->write(chip, 0, reg, data);
} // 0x51

void vgm_parser::do_opn2_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t port = cmd & 0x01;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPN2#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, port, reg, data);
    if(chips.opn2) chips.opn2->write(chip, port, reg, data);
} // 0x52/53

void vgm_parser::do_opm_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPM#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.opm) chips.opm->write(chip, 0, reg, data);
} // 0x54

void vgm_parser::do_opn_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPN#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.opn) chips.opn->write(chip, 0, reg, data);
} // 0x55

void vgm_parser::do_opna_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t port = cmd & 0x01;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPNA#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, port, reg, data);
    if(chips.opna) chips.opna->write(chip, port, reg, data);
} // 0x56/57

void vgm_parser::do_opnb_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t port = cmd & 0x01;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPNB#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, port, reg, data);
    if(chips.opnb) chips.opnb->write(chip, port, reg, data);
} // 0x58/59

void vgm_parser::do_opl2_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPL2#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.opl2) chips.opl2->write(chip, 0, reg, data);
} // 0x5A

void vgm_parser::do_opl_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPL#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.opl) chips.opl->write(chip, 0, reg, data);
} // 0x5B

void vgm_parser::do_msx_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tMSX#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.msx) chips.msx->write(chip, 0, reg, data);
} // 0x5C

void vgm_parser::do_pcmd8_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tPCMD8#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg, data);
    if(chips.pcmd8) chips.pcmd8->write(chip, 0, reg, data);
} // 0x5D

void vgm_parser::do_opl3_write(uint8_t cmd) {
    uint8_t chip = ((cmd & 0xF0) == 0xA0) ? 1 : 0;
    uint8_t port = cmd & 0x01;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOPL3#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, port, reg, data);
    if(chips.opl3) chips.opl3->write(chip, port, reg, data);
} // 0x5E/5F

void vgm_parser::do_wait(uint8_t cmd) {
    uint16_t n;
    if(cmd_log) fprintf(cmd_log, "%02x", cmd);
    switch(cmd) {
        case 0x61:
            n = read_u16le();
            if(cmd_log) fprintf(cmd_log, " %02x %02x", (n & 0xFF), (n >> 8));
            break;
        case 0x62:
            n = 735;
            break;
        case 0x63:
            n = 882;
            break;
        default:
            n = (cmd & 0x0F) + 1;
            break;
    }
    if(cmd_log) fprintf(cmd_log, "\t\twait %u samples\n", n);

    for(size_t samp = 0; samp < n; samp++, _track_pos++, _played_samples++) {
        /* iterate through our list of chips to invoke next_sample() */
        for(size_t i = 0; i < sizeof(chips) / sizeof(chips_array[0]); i++) {
            if(chips_array[i]) chips_array[i]->next_sample();
        }

        if(on_new_sample) {
            if(!on_new_sample(this)) return; // stop processing
        }
    }
} // 0x61/62/63/7n

void vgm_parser::do_end(uint8_t cmd) {
    if(cmd_log) fprintf(cmd_log, "%02x\t\tend of stream\n", cmd);
    if(header.fields.loop_samples) {
        /* we have loops - let's move back to the beginning of the loop section */
        _data_pos = header.fields.loop_offset - header.fields.data_offset;
        _track_pos = header.fields.total_samples - header.fields.loop_samples;
        _played_loops++;
    } else _play_ended = true;
} // 0x66

void vgm_parser::do_data_block(uint8_t cmd) {
    // TODO
} // 0x67

void vgm_parser::do_pcm_ram_write(uint8_t cmd) {
    // TODO
} // 0x68

void vgm_parser::do_opn2_bank_write(uint8_t cmd) {
    uint8_t wait_samples = cmd & 0x0F;
    if(cmd_log) fprintf(cmd_log, "%02x\tOPN2#1\tport 0 reg 0x2A <- data bank + wait %u samples\n", cmd, wait_samples);
    // TODO
} // 0x8n

void vgm_parser::do_dac_stream_setup(uint8_t cmd) {
    // TODO
} // 0x90

void vgm_parser::do_dac_stream_setdata(uint8_t cmd) {
    // TODO
} // 0x91

void vgm_parser::do_dac_stream_setfreq(uint8_t cmd) {
    // TODO
} // 0x92

void vgm_parser::do_dac_stream_start(uint8_t cmd) {
    // TODO
} // 0x93/95

void vgm_parser::do_dac_stream_stop(uint8_t cmd) {
    // TODO
} // 0x94

void vgm_parser::do_ay8910_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tAY8910#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.ay8910) chips.ay8910->write(chip, 0, reg & 0x7F, data);
} // 0xA0

void vgm_parser::do_rf68_write(uint8_t cmd) {
    uint8_t mem = ((cmd & 0xF0) == 0xC0) ? 1 : 0;
    uint16_t addr_reg = (mem) ? read_u16le() : read_u8();
    if(mem) {
        addr_reg = read_u16le();
        if(cmd_log) fprintf(cmd_log, "%02x %02x ", (addr_reg & 0xFF), (addr_reg >> 8));
    } else {
        addr_reg = read_u8();
        if(cmd_log) fprintf(cmd_log, "%02x ", addr_reg);
    }
    uint8_t data = read_u8();

    if(cmd_log) {
        if(mem) fprintf(cmd_log, "%02x %02x %02x %02x\tRF5C68\tmem 0x%04x <- 0x%02x", cmd, (addr_reg & 0xFF), (addr_reg >> 8), data, addr_reg, data);
        else fprintf(cmd_log, "%02x %02x %02x\tRF5C68\treg 0x%02x <- 0x%02x", cmd, addr_reg, data, addr_reg, data);
    }

    if(chips.rf68) chips.rf68->write(0, mem, addr_reg, data);
} // 0xB0/C1

void vgm_parser::do_rf164_write(uint8_t cmd) {
    uint8_t mem = ((cmd & 0xF0) == 0xC0) ? 1 : 0;
    uint16_t addr_reg = (mem) ? read_u16le() : read_u8();
    if(mem) {
        addr_reg = read_u16le();
        if(cmd_log) fprintf(cmd_log, "%02x %02x ", (addr_reg & 0xFF), (addr_reg >> 8));
    } else {
        addr_reg = read_u8();
        if(cmd_log) fprintf(cmd_log, "%02x ", addr_reg);
    }
    uint8_t data = read_u8();

    if(cmd_log) {
        if(mem) fprintf(cmd_log, "%02x %02x %02x %02x\tRF5C164\tmem 0x%04x <- 0x%02x", cmd, (addr_reg & 0xFF), (addr_reg >> 8), data, addr_reg, data);
        else fprintf(cmd_log, "%02x %02x %02x\tRF5C164\treg 0x%02x <- 0x%02x", cmd, addr_reg, data, addr_reg, data);
    }

    if(chips.rf164) chips.rf164->write(0, mem, addr_reg, data);
} // 0xB1/C2

void vgm_parser::do_pwm_write(uint8_t cmd) {
    uint16_t addr_data = read_u16be();
    uint16_t addr = addr_data >> 12, data = addr_data & 0x0FFF;
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tPWM\treg 0x%x <- 0x%03x", cmd, (addr_data >> 8), (addr_data & 0xFF), addr, data);
    if(chips.pwm) chips.pwm->write(0, 0, addr, data);
} // 0xB2

void vgm_parser::do_dmg_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tDMG#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.dmg) chips.dmg->write(chip, 0, reg & 0x7F, data);
} // 0xB3

void vgm_parser::do_apu_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tAPU#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.apu) chips.apu->write(chip, 0, reg & 0x7F, data);
} // 0xB4

void vgm_parser::do_mpcm_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tMultiPCM#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.mpcm) chips.mpcm->write(chip, 0, reg & 0x7F, data);
} // 0xB5

void vgm_parser::do_pd59_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tuPD7759#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.pd59) chips.pd59->write(chip, 0, reg & 0x7F, data);
} // 0xB6

void vgm_parser::do_ok58_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOKIM6258#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.ok58) chips.ok58->write(chip, 0, reg & 0x7F, data);
} // 0xB7

void vgm_parser::do_ok95_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tOKIM6295#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.ok95) chips.ok95->write(chip, 0, reg & 0x7F, data);
} // 0xB8

void vgm_parser::do_hu_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tHuC6280#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.hu) chips.hu->write(chip, 0, reg & 0x7F, data);
} // 0xB9

void vgm_parser::do_k60_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tK053260#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.k60) chips.k60->write(chip, 0, reg & 0x7F, data);
} // 0xBA

void vgm_parser::do_pokey_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tPokey#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.pokey) chips.pokey->write(chip, 0, reg & 0x7F, data);
} // 0xBB

void vgm_parser::do_wswan_write_reg(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tWonderSwan#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.wswan) chips.wswan->write(chip, 0, reg & 0x7F, data);
} // 0xBC

void vgm_parser::do_saa_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tSAA1099#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.saa) chips.saa->write(chip, 0, reg & 0x7F, data);
} // 0xBD

void vgm_parser::do_otto_write(uint8_t cmd) {
    uint8_t write16 = ((cmd & 0xF0) == 0xD0) ? 1 : 0;
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint16_t data = (write16) ? read_u16be() : read_u8();
    if(cmd_log) {
        if(write16) fprintf(cmd_log, "%02x %02x %02x %02x\tOKIM6258#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, (data >> 8), (data & 0xFF), chip + 1, reg & 0x7F, data);
        else fprintf(cmd_log, "%02x %02x %02x\tOKIM6258#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    }
    if(chips.otto) chips.otto->write(chip, write16, reg & 0x7F, data);
} // 0xBE/D6

void vgm_parser::do_ga20_write(uint8_t cmd) {
    uint8_t reg = read_u8();
    uint8_t chip = reg >> 7;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\tGA20#%u\treg 0x%02x <- 0x%02x\n", cmd, reg, data, chip + 1, reg & 0x7F, data);
    if(chips.ga20) chips.ga20->write(chip, 0, reg & 0x7F, data);
} // 0xBF

void vgm_parser::do_spcm_write(uint8_t cmd) {
    uint16_t addr = read_u16le();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tSegaPCM\tmem 0x%04x <- 0x%02x\n", cmd, (addr & 0xFF), (addr >> 8), data, addr, data);
    if(chips.spcm) chips.spcm->write(0, 0, addr, data);
} // 0xC0

void vgm_parser::do_mpcm_write_bank(uint8_t cmd) {
    uint8_t ch = read_u8();
    uint8_t chip = ch >> 7;
    uint8_t data = read_u16le();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tMultiPCM#%u\tch 0x%02x bank offset 0x%04x\n", cmd, ch, (data & 0xFF), (data >> 8), chip + 1, ch & 0x7F, data);
    if(chips.mpcm) chips.mpcm->write(chip, 1, ch & 0x7F, data);
} // 0xC3

void vgm_parser::do_qsound_write(uint8_t cmd) {
    uint16_t data = read_u16be();
    uint8_t reg = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tQSound\treg 0x%02x <- 0x%04x\n", cmd, (data >> 8), (data & 0xFF), reg, reg, data);
    if(chips.qsound) chips.qsound->write(0, 0, reg, data);
} // 0xC4

void vgm_parser::do_scsp_write(uint8_t cmd) {
    uint16_t addr = read_u16be();
    uint8_t chip = addr >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tSCSP#%u\tmem 0x%04x <- 0x%02x\n", cmd, (addr >> 8), (addr & 0xFF), data, chip + 1, addr & 0x7FFF, data);
    if(chips.scsp) chips.scsp->write(chip, 0, addr & 0x7FFF, data);
} // 0xC5

void vgm_parser::do_wswan_write_mem(uint8_t cmd) {
    uint16_t addr = read_u16be();
    uint8_t chip = addr >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tWSwan#%u\tmem 0x%04x <- 0x%02x\n", cmd, (addr >> 8), (addr & 0xFF), data, chip + 1, addr & 0x7FFF, data);
    if(chips.wswan) chips.wswan->write(chip, 0, addr & 0x7FFF, data);
} // 0xC6

void vgm_parser::do_vsu_write(uint8_t cmd) {
    uint16_t addr = read_u16be();
    uint8_t chip = addr >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tVSU#%u\tmem 0x%04x <- 0x%02x\n", cmd, (addr >> 8), (addr & 0xFF), data, chip + 1, addr & 0x7FFF, data);
    if(chips.vsu) chips.vsu->write(chip, 0, addr & 0x7FFF, data);
} // 0xC7

void vgm_parser::do_x1_write(uint8_t cmd) {
    uint16_t addr = read_u16be();
    uint8_t chip = addr >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tX1-010#%u\tmem 0x%04x <- 0x%02x\n", cmd, (addr >> 8), (addr & 0xFF), data, chip + 1, addr & 0x7FFF, data);
    if(chips.x1) chips.x1->write(chip, 0, addr & 0x7FFF, data);
} // 0xC8

void vgm_parser::do_opl4_write(uint8_t cmd) {
    uint8_t port = read_u8();
    uint8_t chip = port >> 7;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tOPL4#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, port, reg, data, chip + 1, port & 0x7F, reg, data);
    if(chips.opl4) chips.opl4->write(chip, port & 0x7F, reg, data);
} // 0xD0

void vgm_parser::do_opx_write(uint8_t cmd) {
    uint8_t port = read_u8();
    uint8_t chip = port >> 7;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tOPX#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, port, reg, data, chip + 1, port & 0x7F, reg, data);
    if(chips.opx) chips.opx->write(chip, port & 0x7F, reg, data);
} // 0xD1

void vgm_parser::do_scc_write(uint8_t cmd) {
    uint8_t port = read_u8();
    uint8_t chip = port >> 7;
    uint8_t reg = read_u8();
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tSCC#%u\tport %u reg 0x%02x <- 0x%02x\n", cmd, port, reg, data, chip + 1, port & 0x7F, reg, data);
    if(chips.scc) chips.scc->write(chip, port & 0x7F, reg, data);
} // 0xD2

void vgm_parser::do_k39_write(uint8_t cmd) {
    uint16_t reg = read_u16be();
    uint8_t chip = reg >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tK054539#%u\treg 0x%04x <- 0x%02x\n", cmd, (reg >> 8), (reg & 0xFF), data, chip + 1, reg & 0x7FFF, data);
    if(chips.k39) chips.k39->write(chip, 0, reg & 0x7FFF, data);
} // 0xD3

void vgm_parser::do_c140_write(uint8_t cmd) {
    uint16_t reg = read_u16be();
    uint8_t chip = reg >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tC140#%u\treg 0x%04x <- 0x%02x\n", cmd, (reg >> 8), (reg & 0xFF), data, chip + 1, reg & 0x7FFF, data);
    if(chips.c140) chips.c140->write(chip, 0, reg & 0x7FFF, data);
} // 0xD4

void vgm_parser::do_doc_write(uint8_t cmd) {
    uint16_t reg = read_u16be();
    uint8_t chip = reg >> 15;
    uint8_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\tDOC#%u\treg 0x%04x <- 0x%02x\n", cmd, (reg >> 8), (reg & 0xFF), data, chip + 1, reg & 0x7FFF, data);
    if(chips.doc) chips.doc->write(chip, 0, reg & 0x7FFF, data);
} // 0xD5

void vgm_parser::do_opn2_bank_seek(uint8_t cmd) {
    uint32_t offset = read_u32le();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x %02x\tOPN2#1\tseek data bank to 0x%08x\n", cmd, ((offset) & 0xFF), ((offset >> 8) & 0xFF), ((offset >> 16) & 0xFF), ((offset >> 24) & 0xFF), offset);
    // TODO
} // 0xE0

void vgm_parser::do_c352_write(uint8_t cmd) {
    uint16_t reg = read_u16be();
    uint8_t chip = reg >> 15;
    uint16_t data = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x %02x\tC352#%u\treg 0x%04x <- 0x%04x\n", cmd, (reg >> 8), (reg & 0xFF), (data >> 8), (data & 0xFF), chip + 1, reg & 0x7FFF, data);
    if(chips.c352) chips.c352->write(chip, 0, reg & 0x7FFF, data);
} // 0xE1

void vgm_parser::do_rsvd_1(uint8_t cmd) {
    uint8_t param = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x\t\treserved\n", cmd, param);
} // 0x30..3F

void vgm_parser::do_rsvd_2(uint8_t cmd) {
    uint8_t param[2]; for(size_t i = 0; i < 2; i++) param[i] = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x\t\treserved\n", cmd, param[0], param[1]);
} // 0x41..4E

void vgm_parser::do_rsvd_3(uint8_t cmd) {
    uint8_t param[3]; for(size_t i = 0; i < 3; i++) param[i] = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x\t\treserved\n", cmd, param[0], param[1], param[2]);
} // 0xC9..CF/D7..DF

void vgm_parser::do_rsvd_4(uint8_t cmd) {
    uint8_t param[4]; for(size_t i = 0; i < 4; i++) param[i] = read_u8();
    if(cmd_log) fprintf(cmd_log, "%02x %02x %02x %02x %02x\t\treserved\n", cmd, param[0], param[1], param[2], param[3]);
} // 0xE2..FF

bool vgm_parser::parse_next() {
    if(!_play_ended) {
        int cmd = read_data();
        if(cmd == EOF) {
            _play_ended = true;
            return false;
        }

        switch(cmd) {
            case 0x31:
                do_ay8910_stereo_mask(cmd);
                break;
            case 0x40:
                do_mikey_write(cmd);
                break;
            case 0x3F:
            case 0x4F:
                do_psg_stereo_write(cmd);
                break;
            case 0x30:
            case 0x50:
                do_psg_write(cmd);
                break;
            case 0x51:
            case 0xA1:
                do_opll_write(cmd);
                break;
            case 0x52:
            case 0x53:
            case 0xA2:
            case 0xA3:
                do_opn2_write(cmd);
                break;
            case 0x54:
            case 0xA4:
                do_opm_write(cmd);
                break;
            case 0x55:
            case 0xA5:
                do_opn_write(cmd);
                break;
            case 0x56:
            case 0x57:
            case 0xA6:
            case 0xA7:
                do_opna_write(cmd);
                break;
            case 0x58:
            case 0x59:
            case 0xA8:
            case 0xA9:
                do_opnb_write(cmd);
                break;
            case 0x5A:
            case 0xAA:
                do_opl2_write(cmd);
                break;
            case 0x5B:
            case 0xAB:
                do_opl_write(cmd);
                break;
            case 0x5C:
            case 0xAC:
                do_msx_write(cmd);
                break;
            case 0x5D:
            case 0xAD:
                do_pcmd8_write(cmd);
                break;
            case 0x5E:
            case 0x5F:
            case 0xAE:
            case 0xAF:
                do_opl3_write(cmd);
                break;
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
            case 0x7E:
            case 0x7F:
                do_wait(cmd);
                break;
            case 0x66:
                do_end(cmd);
                break;
            case 0x67:
                do_data_block(cmd);
                break;
            case 0x68:
                do_pcm_ram_write(cmd);
                break;
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
                do_opn2_bank_write(cmd);
                break;
            case 0x90:
                do_dac_stream_setup(cmd);
                break;
            case 0x91:
                do_dac_stream_setdata(cmd);
                break;
            case 0x92:
                do_dac_stream_setfreq(cmd);
                break;
            case 0x93:
            case 0x95:
                do_dac_stream_start(cmd);
                break;
            case 0x94:
                do_dac_stream_stop(cmd);
                break;
            case 0xA0:
                do_ay8910_write(cmd);
                break;
            case 0xB0:
            case 0xC1:
                do_rf68_write(cmd);
                break;
            case 0xB1:
            case 0xC2:
                do_rf164_write(cmd);
                break;
            case 0xB2:
                do_pwm_write(cmd);
                break;
            case 0xB3:
                do_dmg_write(cmd);
                break;
            case 0xB4:
                do_apu_write(cmd);
                break;
            case 0xB5:
                do_mpcm_write(cmd);
                break;
            case 0xB6:
                do_pd59_write(cmd);
                break;
            case 0xB7:
                do_ok58_write(cmd);
                break;
            case 0xB8:
                do_ok95_write(cmd);
                break;
            case 0xB9:
                do_hu_write(cmd);
                break;
            case 0xBA:
                do_k60_write(cmd);
                break;
            case 0xBB:
                do_pokey_write(cmd);
                break;
            case 0xBC:
                do_wswan_write_reg(cmd);
                break;
            case 0xBD:
                do_saa_write(cmd);
                break;
            case 0xBE:
            case 0xD6:
                do_otto_write(cmd);
                break;
            case 0xBF:
                do_ga20_write(cmd);
                break;
            case 0xC0:
                do_spcm_write(cmd);
                break;
            case 0xC3:
                do_mpcm_write_bank(cmd);
                break;
            case 0xC4:
                do_qsound_write(cmd);
                break;
            case 0xC5:
                do_scsp_write(cmd);
                break;
            case 0xC6:
                do_wswan_write_mem(cmd);
                break;
            case 0xC7:
                do_vsu_write(cmd);
                break;
            case 0xC8:
                do_x1_write(cmd);
                break;
            case 0xD0:
                do_opl4_write(cmd);
                break;
            case 0xD1:
                do_opx_write(cmd);
                break;
            case 0xD2:
                do_scc_write(cmd);
                break;
            case 0xD3:
                do_k39_write(cmd);
                break;
            case 0xD4:
                do_c140_write(cmd);
                break;
            case 0xD5:
                do_doc_write(cmd);
                break;
            case 0xE0:
                do_opn2_bank_seek(cmd);
                break;
            case 0xE1:
                do_c352_write(cmd);
                break;
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3A:
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
                do_rsvd_1(cmd);
                break;
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
                do_rsvd_2(cmd);
                break;
            case 0xC9:
            case 0xCA:
            case 0xCB:
            case 0xCC:
            case 0xCD:
            case 0xCE:
            case 0xCF:
            case 0xD7:
            case 0xD8:
            case 0xD9:
            case 0xDA:
            case 0xDB:
            case 0xDC:
            case 0xDD:
            case 0xDE:
            case 0xDF:
                do_rsvd_3(cmd);
                break;
            default:
                do_rsvd_4(cmd);
                break;
        }
    }

    return !_play_ended;
}
