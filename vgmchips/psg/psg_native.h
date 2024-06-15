#pragma once

#include <vgm_chip.h>
#include <vgm_header.h>
#include <vgm_parser.h>

/* single chip emulator */
class psg_emu {
public:
    psg_emu(size_t clock, sample_t* output_buf, stereo_volume_t* pan_buf, size_t lfsr_width = 16, size_t lfsr_pattern = 0x0009, bool neg_output = false, bool no_gg_stereo = false, bool xnor_noise = false);
    psg_emu(const vgm_header_t& header, sample_t* output_buf, stereo_volume_t* pan_buf);

    const bool& quality = _quality;
    void set_quality(bool q);

    void reset();
    void write_io(uint8_t val);
    void write_stereo(uint8_t val);
    void calc();
private:
    size_t _clock;
    size_t _lfsr_width;
    size_t _lfsr_pattern;
    bool _neg_output;
    bool _no_gg_stereo;
    bool _xnor_noise;

    bool _quality = true;

    uint32_t _base_incr;

    size_t _count[3];
    size_t _volume[3];
    size_t _freq[3];
    size_t _edge[3];
    // size_t _mute[3];

    size_t _noise_seed;
    size_t _noise_count;
    size_t _noise_freq;
    size_t _noise_volume;
    size_t _noise_mode;
    size_t _noise_fref;

    uint32_t _base_count;

    uint32_t _realstep, _sngtime, _sngstep; // rate converter

    uint8_t _addr;
    // size_t _stereo = 0xFF;

    sample_t* _channels_out;
    stereo_volume_t* _channels_pan;

    void internal_refresh();
    void update_output();
};

/* emulator interface */
class psg_native : public vgm_chip {
public:
    psg_native(const vgm_header_t& header);
    ~psg_native();

    virtual void next_sample();
    virtual void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val);
    
    static void install(vgm_parser& parser); // create and plant ourselves in the specified parser object
private:
    const bool _dual_chip;
    psg_emu* _chips[2] = {nullptr, nullptr}; // chips
};