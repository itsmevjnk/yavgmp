#include "parser.h"

/* chip emulators */
#include <psg_native.h>
#include <opll_wrapper.h>
#include <opl_wrapper.h>
#include <opn2_wrapper.h>
#include <emu2149_wrapper.h>
#include <scc_wrapper.h>
#include <rf5c.h>

parser::parser(const char* path, player* p, size_t min_samples) : _min_samples(min_samples), _player(p), _parser(path), gd3_tag(*_parser.gd3_tag), parsed_samples(_parser.played_samples), parsed_loops(_parser.played_loops), parsed_pos(_parser.track_pos), played_samples(p->played_samples), missed_samples(p->missed_samples), total_samples(_parser.header.fields.total_samples), loop_samples(_parser.header.fields.loop_samples) {
    /* install emulators */
    psg_native::install(_parser);
	opll_wrapper::install(_parser);
	opl_wrapper::install(_parser);
	opn2_wrapper::install(_parser);
	emu2149_wrapper::install(_parser);
	scc_wrapper::install(_parser);
	rf5c::install(_parser);
}

void parser::run() {
    while(1) {
        if(_stop_requested) {
            // printf("\nstop requested\n");
			break;
        }

		if(_parser.play_ended) {
			// printf("\nwaiting for player to finish playing\n");
			while(parsed_samples != *((volatile size_t*)&played_samples));
            break;
		}

		_parser.parse_until_next_sample();
		_player->put_sample(_parser.mix_outputs());
		if(!_started && (parsed_samples >= _min_samples || _parser.play_ended)) {
            _player->start(); // we've got enough samples
            _started = true;
        }
    }

    _player->stop();
    _stopped = true; _started = false;
}

std::thread parser::run_thread() {
    return std::thread(&parser::run, this);
}

time_ms_t parser::get_parsed_position() const {
    return vgm_samp_to_minsec(parsed_pos);
}

int parser::get_ahead_samples() const {
    return parsed_samples - played_samples;
}

time_ms_t parser::get_total_duration() const {
    return vgm_samp_to_minsec(total_samples);
}

time_ms_t parser::get_loop_duration() const {
    return vgm_samp_to_minsec(loop_samples);
}