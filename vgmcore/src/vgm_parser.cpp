#include <vgm_parser.h>
#include <zstr.hpp> // for zlib binding

/* allocation chunk size for _data_buf */
#ifndef READ_ALL_CHUNK_SIZE
#define READ_ALL_CHUNK_SIZE                             65536
#endif

void vgm_parser::init_stub(bool read_all) {
    if(read_all) {
        /* read everything into _data_buf, then initialise from there */
        size_t chunks = 0; // number of chunks _data_buf is taking
        size_t file_len = 0;
        do {
            uint8_t* new_buf = (uint8_t*)realloc((void*) _file_buf, ++chunks * READ_ALL_CHUNK_SIZE);
            if(!new_buf) {
                free((void*) _file_buf); _file_buf = nullptr; // in case the destructor is called
                throw std::runtime_error("_data_buf (re)allocation failed");
            }
            _file_buf = new_buf;

            _stream->read((char*)&new_buf[(chunks - 1) * READ_ALL_CHUNK_SIZE], READ_ALL_CHUNK_SIZE);
            file_len += _stream->gcount();
        } while(!_stream->eof());
        
        uint8_t* new_buf = (uint8_t*)realloc((void*) _file_buf, file_len); // shrink our buffer so that we have just enough for our file
        if(new_buf) _file_buf = new_buf; // successful operation - copy our new address over

        delete _stream; _stream = nullptr; // we're done with the stream

        header.init(_file_buf, file_len);
        if(header.fields.gd3_offset) _gd3 = new gd3(&_file_buf[header.fields.gd3_offset], header.fields.eof_offset - header.fields.gd3_offset);

        /* we've effectively cached the entire data stream */
        _data_buf = &_file_buf[header.fields.data_offset];
        _data_buf_cached_bytes = header.get_data_len();
    } else {
        /* initialise straight from stream */
        header.init(*_stream);

        /* try to seek to beginning of data and see if we can actually seek with this stream */
        _stream->seekg(header.fields.data_offset, _stream->beg);
        _stream_seekable = _stream->tellg() == header.fields.data_offset;

        if(_stream_seekable) {
            /* we can seek, so we can get the GD3 tag read */
            if(header.fields.gd3_offset) {
                _stream->seekg(header.fields.gd3_offset, _stream->beg);
                if(_stream->tellg() != header.fields.gd3_offset) throw std::runtime_error("cannot seek to GD3 tag");
                _gd3 = new gd3(*_stream);

                _stream->seekg(header.fields.data_offset, _stream->beg); // seek back to data stream
            }
        } else {
            /* we can't seek :( so no GD3; but we still have to fast forward to our data anyway */
            _stream->clear(); // clear fail bit that might have been set
            for(int i = 0; i < header.fields.data_offset - header.length; i++) _stream->get();

            _data_buf = (uint8_t*)malloc(header.get_data_len()); // allocate space for caching data stream
            if(!_data_buf) throw std::runtime_error("cannot allocate data stream cache");
        }
    }
}

void vgm_parser::init(std::istream& stream, bool read_all) {
    if(!stream) throw std::invalid_argument("stream must be opened");
    _stream = new zstr::istream(stream);

    init_stub(read_all);
}

void vgm_parser::init(const char* path, bool decompress_all) {
    /* check if we need to inflate (decompress) the file at all */
    FILE* file = fopen(path, "r");
    if(!file) throw std::runtime_error("cannot open VGM file");
    uint16_t sig; fread(&sig, 2, 1, file);
    fclose(file);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    bool compressed = (sig == 0x1F8B);
#else
    bool compressed = (sig == 0x8B1F); // little endian - byte order is reversed. anyways, if we've got an uncompressed file then we can just use it as-is (since we can seek)
#endif
    if(!compressed) {
        /* file isn't compressed - we can just open a normal std::ifstream */
        _stream = new std::ifstream(path, std::ios_base::binary);
        decompress_all = false; // no need to read everything out either
    }

    if(!_stream) _stream = new zstr::ifstream(path);
    
    init_stub(decompress_all);

    if(decompress_all) {
        /* we've done reading the file, so let's close it */
        delete _stream;
        _stream = nullptr;
    }
}

vgm_parser::vgm_parser(std::istream& stream, bool read_all) {
    init(stream, read_all);
}

vgm_parser::vgm_parser(const char* path, bool decompress_all) {
    init(path, decompress_all);
}

vgm_parser::~vgm_parser() {
    if(_file_buf) free((void*)_file_buf);
    else if(_data_buf) free((void*)_data_buf); // _data_buf is a separate buffer if _file_buf = nullptr
    if(_gd3) delete _gd3;

    for(size_t i = 0; i < sizeof(chips) / sizeof(chips_array[0]); i++) {
        if(chips_array[i]) delete chips_array[i]; // delete our chip emulators too
    }
}

bool vgm_parser::is_gd3_parsed() const {
    return (_gd3);
}

bool vgm_parser::parse_gd3() {
    if(_gd3) return true; // GD3 tag has already been parsed - skip
    if(!header.fields.gd3_offset) return false; // no GD3 tags for parsing

    populate_cache(); // read the rest of the file into cache
    _gd3 = new gd3(&_data_buf[header.fields.gd3_offset - header.fields.data_offset], header.fields.eof_offset - header.fields.gd3_offset);
    return true;
}

time_ms_t vgm_parser::get_track_position() const {
    return vgm_samp_to_minsec(_track_pos);
}