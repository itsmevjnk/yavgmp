#include <vgm_parser.h>

/* chip volume table - pulled from VGMPlay source code: https://github.com/vgmrips/vgmplay-legacy/blob/e4c884d3518a4ce5a3d9764dcac9f42353192bf4/VGMPlay/VGMPlay.c */
const volume_t vgm_parser::chip_volumes[] = {
    0x80 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x180 / 256.0, 0xB0 / 256.0, 0x100 / 256.0, 0x80 / 256.0,	// 00-07
    0x80 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x98 / 256.0,			// 08-0F
    0x80 / 256.0, 0xE0 / 256.0, 0x100 / 256.0, 0xC0 / 256.0, 0x100 / 256.0, 0x40 / 256.0, 0x11E / 256.0, 0x1C0 / 256.0,		// 10-17
    0x100 / 256.0, 0xA0 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0xB3 / 256.0, 0x100 / 256.0, 0x100 / 256.0,	// 18-1F
    0x20 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x100 / 256.0, 0x40 / 256.0, 0x20 / 256.0, 0x100 / 256.0, 0x40 / 256.0,			// 20-27
    0x280 / 256.0, 0x100 / 256.0
};

stereo_sample_t vgm_parser::mix_outputs() {
    stereo_sample_t result = std::make_pair(0.0, 0.0);
    volume_t absvol = 0; // absolute volume
    for(int i = 0; i < sizeof(chips) / sizeof(vgm_chip*); i++) {
        vgm_chip* chip = chips_array[i];
        if(chip) {
            /* chip is available */
            stereo_sample_t output = chip->mix_channels();
            volume_t volume = chip_volumes[i]; absvol += volume;
            result.first += output.first * volume;
            result.second += output.second * volume;
        }
    }
    if(absvol != 0) {
        /* normalise sample values */
        result.first /= absvol;
        result.second /= absvol;
    }
    return result;
}