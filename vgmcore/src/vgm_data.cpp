#include <vgm_parser.h>

bool vgm_parser::is_data_fully_cached() {
    return (_data_buf_cached_bytes == header.get_data_len());
}

int vgm_parser::read_data() {
    if(_stream && _stream_seekable) {
        /* read directly from stream - no caching needed */
        int data = _stream->get();
        if(data != EOF) _data_pos++;
        return data;
    }

    if(_data_pos < _data_buf_cached_bytes) {
        /* read from cache */
        return _data_buf[_data_pos++];
    } else if(is_data_fully_cached()) return EOF; // no more data, period. 
    else {
        /* we've stepped outside of the cache and there's potentially more stuff to get - let's fetch more data */
        do {
            int data = _stream->get();
            if(data == EOF) break; // womp womp...
            _data_buf[_data_buf_cached_bytes++] = (uint8_t)data;
        } while(_data_buf_cached_bytes <= _data_pos);

        if(_data_buf_cached_bytes > _data_pos) return _data_buf[_data_pos++]; // we've got our data
        return EOF; // we EOF'd before we've got our data
    }
}

void vgm_parser::populate_cache() {
    if((_stream && _stream_seekable) || is_data_fully_cached()) return; // no need to populate cache

    _stream->read((char*)&_data_buf[_data_buf_cached_bytes], header.get_data_len() - _data_buf_cached_bytes);
    _data_buf_cached_bytes += _stream->gcount();
    if(!is_data_fully_cached()) throw std::runtime_error("premature end of file");
    delete _stream; _stream = nullptr; // destroy stream as we no longer need it
}

uint8_t vgm_parser::read_u8() {
    int data = read_data();
    if(data == EOF) throw std::runtime_error("unexpected EOF");
    return (uint8_t) data;
}

uint16_t vgm_parser::read_u16le() {
    uint16_t ret = 0;
    for(size_t i = 0; i < 2; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << (i << 3);
    }
    return ret;
}

uint16_t vgm_parser::read_u16be() {
    uint16_t ret = 0;
    for(size_t i = 0; i < 2; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << ((1 - i) << 3);
    }
    return ret;
}

uint32_t vgm_parser::read_u24le() {
    uint32_t ret = 0;
    for(size_t i = 0; i < 3; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << (i << 3);
    }
    return ret;
}

uint32_t vgm_parser::read_u24be() {
    uint32_t ret = 0;
    for(size_t i = 0; i < 3; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << ((3 - i) << 3);
    }
    return ret;
}

uint32_t vgm_parser::read_u32le() {
    uint32_t ret = 0;
    for(size_t i = 0; i < 4; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << (i << 3);
    }
    return ret;
}

uint32_t vgm_parser::read_u32be() {
    uint32_t ret = 0;
    for(size_t i = 0; i < 4; i++) {
        int data = read_data();
        if(data == EOF) throw std::runtime_error("unexpected EOF");
        ret |= (uint8_t)data << ((3 - i) << 3);
    }
    return ret;
}
