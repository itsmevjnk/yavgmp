#pragma once

#include <stdint.h>
#include <iostream>
#include <functional>

enum psg_flag_bits {
	PSG_FREQ0_400H = (1 << 0),
	PSG_NEG_OUTPUT = (1 << 1),
	PSG_NO_GG_STEREO = (1 << 2),
	PSG_NO_DIV8 = (1 << 3),
	PSG_XNOR_NOISE = (1 << 4)
};
enum ay8910_types {
	AY8910 = 0x00,
	AY8912 = 0x01,
	AY8913 = 0x02,
	AY8930 = 0x03,
	AY8914 = 0x04,
	YM2149 = 0x10,
	YM3439 = 0x11,
	YMZ284 = 0x12,
	YMZ294 = 0x13
};
enum ay8910_flag_bits {
	AY8910_LEGACY_OUT = (1 << 0),
	AY8910_SINGLE_OUT = (1 << 1),
	AY8910_DISCRETE_OUT = (1 << 2),
	AY8910_RAW_OUT = (1 << 3),
	AY8910_YM_PIN26_LOW = (1 << 4)
};

#define VGM_CLOCK(x)												((x) & ~((1 << 30) | (1 << 31))) // get the clock frequency from a clock field, minus the variant and dual chip bits

#pragma pack(push, 1)
typedef struct {
	char ident[4];
	uint32_t eof_offset;

	/* version field broken down here for ease of accessing */
	uint8_t version_min;
	uint8_t version_maj;
	uint16_t version_ext;

	uint32_t psg_clock; // SN76489
	uint32_t opll_clock; // YM2413 (can also be YM2151/YM2612 on VGM <= 1.01)

	uint32_t gd3_offset;

	uint32_t total_samples;
	uint32_t loop_offset;
	uint32_t loop_samples;

	uint32_t rate; // (1.01+)

	uint16_t psg_fb_pattern; // (1.10+)
	uint8_t psg_sr_width; // (1.10+)
	uint8_t psg_flags; // (1.51+) - see psg_flag_bits enum

	uint32_t opn2_clock; // YM2612/3438 (bit 31 on 1.51+ determines if it's a YM3438) (1.10+)
	uint32_t opm_clock; // YM2151/2164 (bit 31 on 1.51+ determines if it's a YM2164) (1.10+)

	uint32_t data_offset; // (1.50+)

	/* 1.51+ */
	uint32_t spcm_clock; // Sega PCM
	uint32_t spcm_if_reg;
	uint32_t rf68_clock; // RF5C68
	uint32_t opn_clock; // YM2203
	uint32_t opna_clock; // YM2608
	uint32_t opnb_clock; // YM2610(B) (bit 31 determines if it's a B variant)
	uint32_t opl2_clock; // YM3812
	uint32_t opl_clock; // YM3526
	uint32_t msx_clock; // Y8950
	uint32_t opl3_clock; // YMF262
	uint32_t opl4_clock; // YMF278B
	uint32_t opx_clock; // YMF271
	uint32_t pcmd8_clock; // YMZ280B
	uint32_t rf164_clock; // RF5C164
	uint32_t pwm_clock; // PWM
	uint32_t ay8910_clock; // AY8910
	uint8_t ay8910_type; // see ay8910_types enum
	uint8_t ay8910_flags; // see ay8910_flag_bits enum
	uint8_t opn_ssg_flags; // for YM2203's internal YM2149
	uint8_t opna_ssg_flags; // for YM2608's internal YM2149

	/* 1.60+ */
	uint8_t volume_mod;
	uint8_t reserved_7d;
	int8_t loop_base;

	uint8_t loop_mod; // (1.51+)

	/* 1.61+ */
	uint32_t dmg_clock; // Game Boy DMG
	uint32_t apu_clock; // NES APU
	uint32_t mpcm_clock; // MultiPCM
	uint32_t pd59_clock; // uPD7759
	uint32_t ok58_clock; // OKIM6258
	uint8_t ok58_flags; // for OKIM6258; see ok58_flag_bits enum
	uint8_t k39_flags; // for K054539; see k39_flag_bits enum
	uint8_t c140_type; // see c140_types enum
	uint8_t reserved_97;
	uint32_t ok95_clock; // OKIM6295
	uint32_t scc_clock; // K051649/K052539 (bit 31 determines if it's a K052539)
	uint32_t k39_clock; // K054539
	uint32_t hu_clock; // HuC6280
	uint32_t c140_clock; // C140
	uint32_t k60_clock; // K053260
	uint32_t pokey_clock; // Pokey
	uint32_t qsound_clock; // QSound

	uint32_t scsp_clock; // SCSP (1.71+)

	uint32_t exthdr_offset; // (1.70+)

	/* 1.71+ */
	uint32_t wswan_clock; // WonderSwan
	uint32_t vsu_clock; // VSU
	uint32_t saa_clock; // SAA1099
	uint32_t doc_clock; // ES5503
	uint32_t otto_clock; // ES5505/ES5506
	uint8_t doc_channels; // for ES5503
	uint8_t otto_channels; // for ES5505/ES5506
	uint8_t c352_clkdiv; // for C352
	uint8_t reserved_d7;
	uint32_t x1_clock; // X1-010
	uint32_t c352_clock; // C352
	uint32_t ga20_clock; // GA20

	uint32_t mikey_clock; // Atari Lynx Mikey (1.72+)
} vgm_header_t;
#pragma pack(pop)

class vgm_header {
public:
	const vgm_header_t& fields = _header;

	void init(const uint8_t* buf, size_t size);
    void init(std::istream& stream);
	void init(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb);

	vgm_header();
	vgm_header(const uint8_t* buf, size_t size);
	vgm_header(std::istream& stream);
	vgm_header(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb);

	bool is_version(int maj, int min) const;
	std::pair<int, float> get_total_duration() const;
	std::pair<int, float> get_loop_duration() const;

	float get_volume_factor() const;

	float get_loop_multiplier() const;

	uint16_t get_c352_clkdiv() const;
private:
	vgm_header_t _header;
    
	template<typename F> void init_stub(F&& read_cb, size_t maxlen);
    
	std::pair<int, float> samples_to_min_sec(uint32_t samples) const;
};