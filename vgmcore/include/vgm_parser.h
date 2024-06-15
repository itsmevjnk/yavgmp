#pragma once

#include <vgm_header.h>
#include <vgm_dblock.h>
#include <vgm_chip.h>
#include <gd3.h>

#include <iostream>
#include <stdio.h>
#include <vector>

class vgm_parser {
public:
    void init(std::istream& stream, bool read_all = true);
    void init(const char* path, bool decompress_all = true);

    vgm_parser(std::istream& stream, bool read_all = true);
    vgm_parser(const char* path, bool decompress_all = true);

    ~vgm_parser();

    bool is_data_fully_cached();

    vgm_header header;

    gd3* const& gd3_tag = _gd3;
    bool is_gd3_parsed() const;
    bool parse_gd3();

    FILE* cmd_log = nullptr; // command logging output

    void populate_cache();

    bool (*on_new_sample)(vgm_parser* parser) = nullptr; // callback for when a new sample has been processed

    /* chips */
    struct {
        vgm_chip* psg = nullptr;
        vgm_chip* opll = nullptr;
        vgm_chip* opn2 = nullptr;
        vgm_chip* opm = nullptr;
        vgm_chip* spcm = nullptr;
        vgm_chip* rf68 = nullptr;
        vgm_chip* opn = nullptr;
        vgm_chip* opna = nullptr;
        vgm_chip* opnb = nullptr;
        vgm_chip* opl2 = nullptr;
        vgm_chip* opl = nullptr;
        vgm_chip* msx = nullptr;
        vgm_chip* opl3 = nullptr;
        vgm_chip* opl4 = nullptr;
        vgm_chip* opx = nullptr;
        vgm_chip* pcmd8 = nullptr;
        vgm_chip* rf164 = nullptr;
        vgm_chip* pwm = nullptr;
        vgm_chip* ay8910 = nullptr;
        vgm_chip* dmg = nullptr;
        vgm_chip* apu = nullptr;
        vgm_chip* mpcm = nullptr;
        vgm_chip* pd59 = nullptr;
        vgm_chip* ok58 = nullptr;
        vgm_chip* ok95 = nullptr;
        vgm_chip* scc = nullptr;
        vgm_chip* k39 = nullptr;
        vgm_chip* hu = nullptr;
        vgm_chip* c140 = nullptr;
        vgm_chip* k60 = nullptr;
        vgm_chip* pokey = nullptr;
        vgm_chip* qsound = nullptr;
        vgm_chip* scsp = nullptr;
        vgm_chip* wswan = nullptr;
        vgm_chip* vsu = nullptr;
        vgm_chip* saa = nullptr;
        vgm_chip* doc = nullptr;
        vgm_chip* otto = nullptr;
        vgm_chip* c352 = nullptr;
        vgm_chip* x1 = nullptr;
        vgm_chip* ga20 = nullptr;
        vgm_chip* mikey = nullptr;
    } chips;
    vgm_chip* const* const chips_array = (vgm_chip* const* const)&chips;

    const size_t& track_pos = _track_pos;
    const size_t& played_samples = _played_samples;
    const size_t& played_loops = _played_loops;

    pif get_track_position() const;

    const bool& play_ended = _play_ended;

    bool process_wait(size_t n = 1); // process pending wait up to n samples
    bool parse_next(); // parse next command or get next sample, returns inverse of play_ended value (i.e. whether there's still more stuff to be played)
    bool parse_until_next_sample(size_t n = 1); // parse until the next n samples has been played
    const int& pending_samples = _pending_samples;

    const std::vector<vgm_dblock_t const*>& data_blocks = _data_blocks;
    const std::vector<vgm_dblock_t const*>& decomp_tables = _decomp_tables;

    static const float chip_volumes[];
    pff mix_outputs();
private:
    void init_stub(bool read_all);
    
    gd3* _gd3 = nullptr; // null if GD3 tag isn't parsed

    std::istream* _stream = nullptr; // null if data is to be read from _data_buf instead
    bool _stream_seekable = false; // set if _stream is seekable (N/A if _stream = nullptr). caching will be disabled if _stream_seekable = true
    uint8_t* _file_buf = nullptr; // buffer of entire file contents (if read_all = true)

    uint8_t* _data_buf = nullptr; // buffer of data stream only (can be either part of _file_buf if _file_buf != nullptr, or a separate buffer otherwise)
    size_t _data_buf_cached_bytes = 0; // number of bytes cached in _data_buf

    size_t _data_pos = 0; // current offset into data stream
    size_t _track_pos = 0; // data position in samples

    size_t _played_samples = 0; // total number of samples played
    size_t _played_loops = 0; // total number of loops played
    
    int _pending_samples = 0; // number of samples to wait before processing next command

    bool _play_ended = false; // set when there's no more stuff to be played in this file

    std::vector<vgm_dblock_t const*> _data_blocks;
    std::vector<vgm_dblock_t const*> _decomp_tables;

    /* command data reading helpers */
    int read_data();
    uint8_t read_u8(); // NOTE: read_u8(), read_u16le(), read_u16be(), read_u32le() and read_u32ne() will throw runtime errors if there's an unexpected EOF
    uint16_t read_u16le();
    uint16_t read_u16be();
    uint32_t read_u24le();
    uint32_t read_u24be();
    uint32_t read_u32le();
    uint32_t read_u32be();

    /* command handlers */
    void do_ay8910_stereo_mask(uint8_t cmd); // 0x31
    void do_mikey_write(uint8_t cmd); // 0x40
    void do_psg_stereo_write(uint8_t cmd); // 0x4F
    void do_psg_write(uint8_t cmd); // 0x50
    void do_opll_write(uint8_t cmd); // 0x51
    void do_opn2_write(uint8_t cmd); // 0x52/53
    void do_opm_write(uint8_t cmd); // 0x54
    void do_opn_write(uint8_t cmd); // 0x55
    void do_opna_write(uint8_t cmd); // 0x56/57
    void do_opnb_write(uint8_t cmd); // 0x58/59
    void do_opl2_write(uint8_t cmd); // 0x5A
    void do_opl_write(uint8_t cmd); // 0x5B
    void do_msx_write(uint8_t cmd); // 0x5C
    void do_pcmd8_write(uint8_t cmd); // 0x5D
    void do_opl3_write(uint8_t cmd); // 0x5E/5F
    void do_wait(uint8_t cmd); // 0x61/62/63/7n
    void do_end(uint8_t cmd); // 0x66
    void do_data_block(uint8_t cmd); // 0x67
    void do_pcm_ram_write(uint8_t cmd); // 0x68
    void do_opn2_bank_write(uint8_t cmd); // 0x8n
    void do_dac_stream_setup(uint8_t cmd); // 0x90
    void do_dac_stream_setdata(uint8_t cmd); // 0x91
    void do_dac_stream_setfreq(uint8_t cmd); // 0x92
    void do_dac_stream_start(uint8_t cmd); // 0x93/95
    void do_dac_stream_stop(uint8_t cmd); // 0x94
    void do_ay8910_write(uint8_t cmd); // 0xA0
    void do_rf68_write(uint8_t cmd); // 0xB0/C1
    void do_rf164_write(uint8_t cmd); // 0xB1/C2
    void do_pwm_write(uint8_t cmd); // 0xB2
    void do_dmg_write(uint8_t cmd); // 0xB3
    void do_apu_write(uint8_t cmd); // 0xB4
    void do_mpcm_write(uint8_t cmd); // 0xB5
    void do_pd59_write(uint8_t cmd); // 0xB6
    void do_ok58_write(uint8_t cmd); // 0xB7
    void do_ok95_write(uint8_t cmd); // 0xB8
    void do_hu_write(uint8_t cmd); // 0xB9
    void do_k60_write(uint8_t cmd); // 0xBA
    void do_pokey_write(uint8_t cmd); // 0xBB
    void do_wswan_write_reg(uint8_t cmd); // 0xBC
    void do_saa_write(uint8_t cmd); // 0xBD
    void do_otto_write(uint8_t cmd); // 0xBE/D6
    void do_ga20_write(uint8_t cmd); // 0xBF
    void do_spcm_write(uint8_t cmd); // 0xC0
    void do_mpcm_write_bank(uint8_t cmd); // 0xC3
    void do_qsound_write(uint8_t cmd); // 0xC4
    void do_scsp_write(uint8_t cmd); // 0xC5
    void do_wswan_write_mem(uint8_t cmd); // 0xC6
    void do_vsu_write(uint8_t cmd); // 0xC7
    void do_x1_write(uint8_t cmd); // 0xC8
    void do_opl4_write(uint8_t cmd); // 0xD0
    void do_opx_write(uint8_t cmd); // 0xD1
    void do_scc_write(uint8_t cmd); // 0xD2
    void do_k39_write(uint8_t cmd); // 0xD3
    void do_c140_write(uint8_t cmd); // 0xD4
    void do_doc_write(uint8_t cmd); // 0xD5
    void do_opn2_bank_seek(uint8_t cmd); // 0xE0
    void do_c352_write(uint8_t cmd); // 0xE1
    void do_rsvd_1(uint8_t cmd); // 0x30..3F
    void do_rsvd_2(uint8_t cmd); // 0x41..4E
    void do_rsvd_3(uint8_t cmd); // 0xC9..CF/D7..DF
    void do_rsvd_4(uint8_t cmd); // 0xE2..FF

    // void wait(size_t n);

    size_t _opn2_bank_offset = 0; // for 0x8n and 0xE0
};
