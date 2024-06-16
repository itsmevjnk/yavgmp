/* parser (sample producer) */

#pragma once

#include "player.h"

#include <vgm_parser.h>
#include <thread>

class parser {
public:
    parser(const char* path, player* p, size_t min_samples = 2205); // create parser object opening the specified VGM file (decompressing into memory beforehand for VGZ files) and attaching to specified player instance
private:
    player* _player; // attached player object (which we'll feed samples into)
    vgm_parser _parser; // VGM file parser
public:
    void run(); // run parser (blocking)
    std::thread run_thread(); // start new thread for this parser and return it (must be consumed by caller!)

    void stop(); // request run thread to stop playback

    const gd3& gd3_tag; // file's GD3 tag

    const uint32_t& total_samples;
    const uint32_t& loop_samples;

    time_ms_t get_total_duration() const;
    time_ms_t get_loop_duration() const;

    const size_t& parsed_samples;
    const size_t& parsed_loops;
    const size_t& parsed_pos;

    const size_t& played_samples;
    const size_t& missed_samples;
    
    time_ms_t get_parsed_position() const;
    int get_ahead_samples() const;

    const volatile bool& stopped = _stopped;
private:
    bool _started = false; // set when player has been started
    bool _stop_requested = false;
    bool _stopped = false;

    const size_t _min_samples;
};
