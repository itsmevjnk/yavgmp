#include <gd3.h>
#include <string.h>

template<typename F> size_t gd3::read_utf16le_string(F&& read_cb, size_t offset, size_t maxlen, std::string& dest) {
    uint16_t c;
    do {
        if(offset >= maxlen) throw std::runtime_error("unexpected end of file");
        read_cb((uint8_t*)&c, offset, 2); offset += 2;
        if(!c) break; // we're done reading
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	    c = ((c & 0xFF00) >> 8) | ((c & 0x00FF) << 8);
#endif
        /* UTF16 to UTF8 */
        if(c < 0x80) dest += (char)c;
        else if(c < 0x800) {
            dest += (char)(0b11000000 | (c >> 6));
            dest += (char)(0b10000000 | (c & 0b00111111));
        } else {
            dest += (char)(0b11100000 | (c >> 12));
            dest += (char)(0b10000000 | ((c >> 6) & 0b00111111));
            dest += (char)(0b10000000 | (c & 0b00111111));
        }
    } while(c);
    return offset;
}

template<typename F> void gd3::init_stub(F&& read_cb, size_t maxlen) {
    /* check ident */
    uint8_t temp[4]; read_cb(temp, 0, 4);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	if(*((uint32_t*)&temp) != 0x47643320)
#else
	if(*((uint32_t*)&temp) != 0x20336447)
#endif
		throw std::invalid_argument("invalid ident");
    
    read_cb(temp, 4, 4); _ver_min = temp[0]; _ver_maj = temp[1]; // read version
    
    /* read data length field */
    uint32_t len; read_cb((uint8_t*)&len, 8, 4);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    len = ((len & 0xFF000000) >> 24) | ((len & 0x00FF0000) >> 8) | ((len & 0x0000FF00) << 8) | ((len & 0x000000FF) << 24);
#endif
    len += 12;
    if(len < maxlen) maxlen = len;

    /* read string fields */
    size_t offset = 12;
    offset = read_utf16le_string(read_cb, offset, maxlen, _title);
    offset = read_utf16le_string(read_cb, offset, maxlen, _title_orig);
    offset = read_utf16le_string(read_cb, offset, maxlen, _game);
    offset = read_utf16le_string(read_cb, offset, maxlen, _game_orig);
    offset = read_utf16le_string(read_cb, offset, maxlen, _sys);
    offset = read_utf16le_string(read_cb, offset, maxlen, _sys_orig);
    offset = read_utf16le_string(read_cb, offset, maxlen, _author);
    offset = read_utf16le_string(read_cb, offset, maxlen, _author_orig);
    offset = read_utf16le_string(read_cb, offset, maxlen, _date);
    offset = read_utf16le_string(read_cb, offset, maxlen, _author_conv);
    offset = read_utf16le_string(read_cb, offset, maxlen, _notes);
}

void gd3::init(const uint8_t* buf, size_t size) {
	if(!buf) throw std::invalid_argument("buf cannot be null");
    if(size < 4 + 4 + 2 * 11) throw std::invalid_argument("buf is too small to be GD3 tag");

    init_stub(
        [&buf](uint8_t* dest_buf, size_t offset, size_t len) {
            memcpy(dest_buf, &buf[offset], len);
        },
        size
    );
}

void gd3::init(std::istream& stream) {
	if(!stream) throw std::invalid_argument("stream cannot be null");

    size_t current_offset = 0;
    init_stub(
        [&stream, &current_offset](uint8_t* dest_buf, size_t offset, size_t len) {
            if(current_offset != offset) throw std::runtime_error("unexpected non-sequential read");
			stream.read((char*)dest_buf, len);
			if(stream.fail()) throw std::runtime_error("read operation failed");
			current_offset += len;
        },
        SIZE_MAX
    );
}

void gd3::init(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb) {
	if(!read_cb) throw std::invalid_argument("read_cb cannot be null");

    init_stub(
        [&read_cb](uint8_t* dest_buf, size_t offset, size_t len) {
            if(!read_cb(dest_buf, offset, len)) throw std::runtime_error("read_cb reported error");
        },
        SIZE_MAX
    );
}

gd3::gd3() {

}

gd3::gd3(const uint8_t* buf, size_t size) {
    init(buf, size);
}
gd3::gd3(std::istream& stream) {
    init(stream);
}
gd3::gd3(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb) {
    init(read_cb);
}