#include <vgm_header.h>
#include <string.h>
#include <math.h>

#define BCD_TO_INT(x)						((uint8_t)((x) - 6 * ((x) >> 4))) // convert BCD to integer (https://stackoverflow.com/a/42340213)
#define RELOFF_TO_ABSOFF(base, x)			((x) + (uint32_t)(((uintptr_t)&x) - ((uintptr_t)&base))) // convert relative to absolute offset

/* endianness flip macros (big <-> little) */
#define ENDIAN_FLIP_16(x)					((((x) & 0x00FF) << 8) | (((x) & 0xFF00) >> 8))
#define ENDIAN_FLIP_32(x)					((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

bool vgm_header::is_version(int maj, int min) const {
	if(fields.version_maj < maj) return false; // less than major version
	if(fields.version_maj > maj) return true; // newer than given major version
	return (fields.version_min >= min); // same major version - check minor version
}

template<typename F>
void vgm_header::init_stub(F&& read_cb, size_t maxlen) {
	memset(&_header, 0, sizeof(_header)); // clear header buffer
	read_cb((uint8_t*)&_header, 0, 0x40); // copy first 64 bytes of the header (which must exist no matter what)

	// if(memcmp(&_header.ident, "Vgm ", 4))
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	if(*((uint32_t*)&_header.ident) != 0x56676D20)
#else
	if(*((uint32_t*)&_header.ident) != 0x206D6756)
#endif
		throw std::invalid_argument("invalid ident");

	_header.version_maj = BCD_TO_INT(_header.version_maj); _header.version_min = BCD_TO_INT(_header.version_min); // convert version header from BCD to integer

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ // do little -> big endian flips
	_header.eof_offset = ENDIAN_FLIP_32(_header.eof_offset);
	_header.psg_clock = ENDIAN_FLIP_32(_header.psg_clock);
	_header.opll_clock = ENDIAN_FLIP_32(_header.opll_clock);
	_header.gd3_offset = ENDIAN_FLIP_32(_header.gd3_offset);
	_header.total_samples = ENDIAN_FLIP_32(_header.total_samples);
	_header.loop_offset = ENDIAN_FLIP_32(_header.loop_offset);
	_header.loop_samples = ENDIAN_FLIP_32(_header.loop_samples);
#endif
	_header.eof_offset = RELOFF_TO_ABSOFF(_header, _header.eof_offset); // convert EOF offset field from relative offset to absolute offset
	_header.gd3_offset = RELOFF_TO_ABSOFF(_header, _header.gd3_offset);
	_header.loop_offset = RELOFF_TO_ABSOFF(_header, _header.loop_offset);

	if(_header.loop_offset >= _header.eof_offset) {
		/* invalid loop offset - treat as no loop */
		_header.loop_offset = 0;
		_header.loop_samples = 0;
	}

	/* 1.01 */
	if(!is_version(1, 01)) _header.rate = 0;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	else _header.rate = ENDIAN_FLIP_32(_header.rate);
#endif

	/* 1.10 */
	if(!is_version(1, 10)) {
		_header.psg_fb_pattern = 0x0009;
		_header.psg_sr_width = 16;
		_header.opn2_clock = (_header.opll_clock > 5000000) ? _header.opll_clock : 0;
		_header.opm_clock = (_header.opll_clock < 5000000) ? _header.opll_clock : 0;
	}
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	else {
		_header.psg_fb_pattern = ENDIAN_FLIP_16(_header.psg_fb_pattern);
		_header.opn2_clock = ENDIAN_FLIP_32(_header.opn2_clock);
		_header.opm_clock = ENDIAN_FLIP_32(_header.opm_clock);
	}
#endif

	/* 1.50 */
	if(!is_version(1, 50)) {
		_header.data_offset = 0x40;
		_header.volume_mod = 0; // NOTE: this is a 1.60+ addition, but vgmrips.net's VGM specs said it should be supported on 1.50+?
	}
	else {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		_header.data_offset = ENDIAN_FLIP_32(_header.data_offset);
#endif
		_header.data_offset = RELOFF_TO_ABSOFF(_header, _header.data_offset);
		if(maxlen > 0x40) {
			/* read the rest of the header */
			size_t len = sizeof(_header); // offset of end of header
			if(_header.data_offset < len) len = _header.data_offset;
			if(maxlen < len) len = maxlen; // TODO: should we throw an exception here?
			_header_len = len;
			read_cb((uint8_t*)((uintptr_t)&_header + 0x40), 0x40, len - 0x40);
		}
	}

	/* 1.51 */
	if(!is_version(1, 51)) {
		_header.psg_flags = 0;
		_header.opn2_clock &= ~(1 << 31); // this bit shouldn't be on on VGM < 1.51
		_header.opm_clock &= ~(1 << 31);
		_header.loop_mod = 0;
	}
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	else {
		_header.spcm_clock = ENDIAN_FLIP_32(_header.spcm_clock);
		_header.spcm_if_reg = ENDIAN_FLIP_32(_header.spcm_if_reg);
		_header.rf68_clock = ENDIAN_FLIP_32(_header.rf68_clock);
		_header.opn_clock = ENDIAN_FLIP_32(_header.opn_clock);
		_header.opna_clock = ENDIAN_FLIP_32(_header.opna_clock);
		_header.opnb_clock = ENDIAN_FLIP_32(_header.opnb_clock);
		_header.opl2_clock = ENDIAN_FLIP_32(_header.opl2_clock);
		_header.opl_clock = ENDIAN_FLIP_32(_header.opl_clock);
		_header.msx_clock = ENDIAN_FLIP_32(_header.msx_clock);
		_header.opl3_clock = ENDIAN_FLIP_32(_header.opl3_clock);
		_header.opl4_clock = ENDIAN_FLIP_32(_header.opl4_clock);
		_header.opx_clock = ENDIAN_FLIP_32(_header.opx_clock);
		_header.pcmd8_clock = ENDIAN_FLIP_32(_header.pcmd8_clock);
		_header.rf164_clock = ENDIAN_FLIP_32(_header.rf164_clock);
		_header.pwm_clock = ENDIAN_FLIP_32(_header.pwm_clock);
		_header.ay8910_clock = ENDIAN_FLIP_32(_header.ay8910_clock);
	}
#endif
	if(!_header.loop_mod) _header.loop_mod = 0x10;

	/* 1.60 */
	if(!is_version(1, 60)) _header.loop_base = 0;

	/* 1.61 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	if(is_version(1, 61)) {
		_header.dmg_clock = ENDIAN_FLIP_32(_header.dmg_clock);
		_header.apu_clock = ENDIAN_FLIP_32(_header.apu_clock);
		_header.mpcm_clock = ENDIAN_FLIP_32(_header.mpcm_clock);
		_header.pd59_clock = ENDIAN_FLIP_32(_header.pd59_clock);
		_header.ok58_clock = ENDIAN_FLIP_32(_header.ok58_clock);
		_header.ok95_clock = ENDIAN_FLIP_32(_header.ok95_clock);
		_header.scc_clock = ENDIAN_FLIP_32(_header.scc_clock);
		_header.k39_clock = ENDIAN_FLIP_32(_header.k39_clock);
		_header.hu_clock = ENDIAN_FLIP_32(_header.hu_clock);
		_header.c140_clock = ENDIAN_FLIP_32(_header.c140_clock);
		_header.k60_clock = ENDIAN_FLIP_32(_header.k60_clock);
		_header.pokey_clock = ENDIAN_FLIP_32(_header.pokey_clock);
		_header.qsound_clock = ENDIAN_FLIP_32(_header.qsound_clock);
	}
#endif

	/* 1.71 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	if(is_version(1, 71)) {
		_header.scsp_clock = ENDIAN_FLIP_32(_header.scsp_clock);
		_header.wswan_clock = ENDIAN_FLIP_32(_header.wswan_clock);
		_header.vsu_clock = ENDIAN_FLIP_32(_header.vsu_clock);
		_header.saa_clock = ENDIAN_FLIP_32(_header.saa_clock);
		_header.doc_clock = ENDIAN_FLIP_32(_header.doc_clock);
		_header.otto_clock = ENDIAN_FLIP_32(_header.otto_clock);
		_header.x1_clock = ENDIAN_FLIP_32(_header.x1_clock);
		_header.c352_clock = ENDIAN_FLIP_32(_header.c352_clock);
		_header.ga20_clock = ENDIAN_FLIP_32(_header.ga20_clock);
		_header.scsp_clock = ENDIAN_FLIP_32(_header.scsp_clock);
	}
#endif

	/* 1.70 */
	if(!is_version(1, 70)) _header.exthdr_offset = 0;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	else _header.exthdr_offset = ENDIAN_FLIP_32(_header.exthdr_offset);
#endif

	/* 1.72 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	if(is_version(1, 72)) _header.mikey_clock = ENDIAN_FLIP_32(_header.mikey_clock);
#endif
}

void vgm_header::init(const uint8_t* buf, size_t size) {
	if(!buf) throw std::invalid_argument("buf cannot be null");
	if(size < 0x40) throw std::invalid_argument("buf must be at least 64 bytes in size - is this a valid VGM file?");

	init_stub(
		[buf](uint8_t* dest_buf, size_t offset, size_t len) {
			memcpy(dest_buf, &buf[offset], len);
		},
		size
	);
}

vgm_header::vgm_header(const uint8_t* buf, size_t size) {
	init(buf, size);
}

void vgm_header::init(std::istream& stream) {
	if(!stream) throw std::invalid_argument("stream cannot be null");

	size_t current_offset = 0;
	init_stub(
		[&stream, &current_offset](uint8_t* dest_buf, size_t offset, size_t len) {
			if(current_offset != offset) throw std::runtime_error("unexpected non-sequential read");
			stream.read((char*)dest_buf, len);
			if(stream.fail()) throw std::runtime_error("read operation failed");
			current_offset += len;
		},
		SIZE_MAX // we don't know how big our stream is
	);
}

vgm_header::vgm_header(std::istream& stream) {
	init(stream);
}

void vgm_header::init(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb) {
	if(!read_cb) throw std::invalid_argument("read_cb cannot be null");
	init_stub(
		[&read_cb](uint8_t* dest_buf, size_t offset, size_t len) {
			if(!read_cb(dest_buf, offset, len)) throw std::runtime_error("read_cb reported reading error");
		},
		SIZE_MAX
	);
}

vgm_header::vgm_header(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb) {
	init(read_cb);
}

vgm_header::vgm_header() {

}

std::pair<int, float> vgm_header::samples_to_min_sec(uint32_t samples) const {
	float dur_seconds = samples / 44100.0;
	int min = (int)dur_seconds / 60;
	float sec = dur_seconds - min * 60;
	return std::make_pair(min, sec);
}

std::pair<int, float> vgm_header::get_total_duration() const {
	return samples_to_min_sec(fields.total_samples);
}

std::pair<int, float> vgm_header::get_loop_duration() const {
	return samples_to_min_sec(fields.loop_samples);
}

float vgm_header::get_volume_factor() const {
	int mod = fields.volume_mod; if(mod > 0xC0) mod = (mod == 0xC1) ? -64 : (mod - 256);
	return pow(2, (float)mod / 0x20);
}

float vgm_header::get_loop_multiplier() const {
	return (float)fields.loop_mod / 0x10;
}

uint16_t vgm_header::get_c352_clkdiv() const {
	if(!fields.c352_clock) return 0; // C352 not present
	return (uint16_t)fields.c352_clkdiv << 2;
}

size_t vgm_header::get_data_len() const {
	return fields.eof_offset - fields.data_offset;
}