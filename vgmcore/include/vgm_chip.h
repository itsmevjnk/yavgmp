#pragma once

#include <vgm_types.h>
#include <vector>

/* chip IDs */
enum vgm_chip_ids {
    VGM_CID_PSG = 0x00,
    VGM_CID_OPLL,
    VGM_CID_OPN2,
    VGM_CID_OPM,
    VGM_CID_SPCM,
    VGM_CID_RF68,
    VGM_CID_OPN,
    VGM_CID_OPNA,
    VGM_CID_OPNB,
    VGM_CID_OPL2,
    VGM_CID_OPL,
    VGM_CID_MSX,
    VGM_CID_OPL3,
    VGM_CID_OPL4,
    VGM_CID_OPX,
    VGM_CID_PCMD8,
    VGM_CID_RF164,
    VGM_CID_PWM,
    VGM_CID_AY8910,
    VGM_CID_DMG,
    VGM_CID_APU,
    VGM_CID_MPCM,
    VGM_CID_PD59,
    VGM_CID_OK58,
    VGM_CID_OK95,
    VGM_CID_SCC,
    VGM_CID_K39,
    VGM_CID_HU,
    VGM_CID_C140,
    VGM_CID_K60,
    VGM_CID_POKEY,
    VGM_CID_QSOUND,
    VGM_CID_SCSP,
    VGM_CID_WSWAN,
    VGM_CID_VSU,
    VGM_CID_SAA,
    VGM_CID_DOC,
    VGM_CID_OTTO,
    VGM_CID_C352,
    VGM_CID_X1,
    VGM_CID_GA20,
    VGM_CID_MIKEY,
    VGM_CID_END
};

class vgm_chip {
public:
    vgm_chip(size_t num_channels, bool stereo_channels = false);
    ~vgm_chip();

    virtual void next_sample() = 0;
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) = 0;
    virtual void write_rom(uintptr_t chip, size_t total_size, uintptr_t base_addr, const uint8_t* data, size_t len, size_t type = 0);
    virtual void write_ram(uintptr_t chip, uintptr_t base_addr, const uint8_t* data, size_t len);
    const size_t channels; // number of channels
    const sample_t* channels_out_left, *channels_out_right; // channel outputs
    const stereo_volume_t* channels_pan; // channel levels per output (left, right) - for mixing
    const bool stereo; // set if channels are stereo
    virtual stereo_sample_t mix_channels(); // mix channels into stereo outputs (NOTE: chips can provide their own implementations too - eg. OPN2 with its TDM mixing)
protected:
    sample_t* _channels_out = nullptr; // only set if stereo_channels = false
    sample_t* _channels_out_left = nullptr, *_channels_out_right = nullptr; // set to _channels_out if stereo_channels = false; their own buffers otherwise (and _channels_out would be null)
    stereo_volume_t* _channels_pan;
};