#pragma once

#include <iostream>
#include <functional>

class gd3 {
public:
    void init(const uint8_t* buf, size_t size);
    void init(std::istream& stream);
	void init(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb);

    gd3();
	gd3(const uint8_t* buf, size_t size);
	gd3(std::istream& stream);
	gd3(std::function<bool(uint8_t* dest_buf, size_t offset, size_t len)> read_cb);

    const uint8_t& version_maj = _ver_maj;
    const uint8_t& version_min = _ver_min;

    const std::string& title = _title;
    const std::string& original_title = _title_orig;
    const std::string& game = _game;
    const std::string& original_game = _game_orig;
    const std::string& system = _sys;
    const std::string& original_system = _sys_orig;
    const std::string& author = _author;
    const std::string& original_author = _author_orig;
    const std::string& date = _date;
    const std::string& convert_author = _author_conv;
    const std::string& notes = _notes;
private:
    std::string _title;
    std::string _title_orig;
    std::string _game;
    std::string _game_orig;
    std::string _sys;
    std::string _sys_orig;
    std::string _author;
    std::string _author_orig;
    std::string _date;
    std::string _author_conv;
    std::string _notes;

    uint8_t _ver_maj = 0, _ver_min = 0;

    template<typename F> size_t read_utf16le_string(F&& read_cb, size_t offset, size_t maxlen, std::string& dest);
    template<typename F> void init_stub(F&& read_cb, size_t maxlen);
};