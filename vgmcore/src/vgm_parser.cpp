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

        _data_buf = &_file_buf[header.fields.data_offset]; _data_buf_cached_bytes = header.get_data_len(); // we've effectively cached the entire data stream
    } else {
        /* initialise straight from stream */
        header.init(*_stream);

        /* try to seek to beginning of data and see if we can actually seek with this stream */
        _stream->seekg(header.fields.data_offset, _stream->beg);
        bool seekable = _stream->tellg() == header.fields.data_offset;

        if(seekable) {
            /* we can seek, so we can get the GD3 tag read */
            if(header.fields.gd3_offset) {
                _stream->seekg(header.fields.gd3_offset, _stream->beg);
                if(_stream->tellg() != header.fields.gd3_offset) throw std::runtime_error("cannot seek to GD3 tag");
                _gd3 = new gd3(*_stream);

                _stream->seekg(header.fields.data_offset, _stream->beg); // seek back to data stream
            }
        } else {
            /* we can't seek :( so no GD3; but we still have to fast forward to our data anyway */
            for(int i = 0; i < header.fields.data_offset - header.length; i++) _stream->get();
        }

        _data_buf = (uint8_t*)malloc(header.get_data_len()); // allocate space for caching data stream
        if(!_data_buf) throw std::runtime_error("cannot allocate data stream cache");
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
}

bool vgm_parser::is_data_fully_cached() const {
    return (_file_buf || (_data_buf_cached_samples == header.fields.total_samples && _data_buf[_data_buf_cached_bytes - 1] == 0x66));
    /*
     * we can consider the data to be fully cached if either
     * _file_buf is not null (meaning we've read everything
     * when we initialised), or we've cached (played) the
     * exact number of samples as the file has and the last
     * byte in our cache is data stream termination command
     * (0x66)
     */
}