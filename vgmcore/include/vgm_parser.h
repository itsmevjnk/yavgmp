#pragma once

#include <vgm_header.h>
#include <gd3.h>

#include <iostream>

class vgm_parser {
public:
    void init(std::istream& stream, bool read_all = true);
    void init(const char* path, bool decompress_all = true);

    vgm_parser(std::istream& stream, bool read_all = true);
    vgm_parser(const char* path, bool decompress_all = true);

    ~vgm_parser();

    bool is_data_fully_cached() const;

    vgm_header header;
    gd3* const& gd3_tag = _gd3;
private:
    void init_stub(bool read_all);
    
    gd3* _gd3 = nullptr; // null if GD3 tag isn't parsed

    std::istream* _stream = nullptr; // null if data is to be read from _data_buf instead
    uint8_t* _file_buf = nullptr; // buffer of entire file contents (if read_all = true)

    uint8_t* _data_buf = nullptr; // buffer of data stream only (can be either part of _file_buf if _file_buf != nullptr, or a separate buffer otherwise)
    size_t _data_buf_cached_bytes = 0; // number of bytes cached in _data_buf
    size_t _data_buf_cached_samples = 0; // number of samples cached in _data_buf
};
