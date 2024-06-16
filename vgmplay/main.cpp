#include "types.h"
#include "player.h"
#include "parser.h"

#include <vgm_parser.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define PLAYER_SAMPLE_BUF_START_SIZE								(PLAYER_SAMPLE_BUF_SIZE / 2) // number of samples to pre-enqueue

int main(int argc, char* argv[]) {
	opterr = 0;
	int c;
	while((c = getopt(argc, argv, "")) != -1) {
		switch(c) {
			case '?':
				if(isprint(optopt))
					fprintf(stderr, "vgmplay: unknown option -%c\n", optopt);
				else
					fprintf(stderr, "vgmplay: unknown character 0x%x\n", optopt);
				return 1;
			default:
				abort();
		}		
	}
	
	if(optind == argc) {
		fprintf(stderr, "vgmplay: no input file given\n");
		return 1;
	}
	const char* infile = argv[optind];

	/* set up playing */
	player player_obj;
	parser parser_obj(infile, &player_obj);

	time_ms_t total_dur = parser_obj.get_total_duration();
	printf("playing VGM file: %s (%02d:%06.3f%s)\n", infile, total_dur.min, total_dur.sec, parser_obj.loop_samples ? ", looped" : "");
	std::thread parser_thread = parser_obj.run_thread();
	printf("started parser thread (ID %u)\n", parser_thread.native_handle());
	while(!parser_obj.stopped) {
		time_ms_t proc_pos = parser_obj.get_parsed_position();

		time_ms_t proc_time = vgm_samp_to_minsec(parser_obj.parsed_samples);
		time_ms_t play_time = vgm_samp_to_minsec(parser_obj.played_samples);
		printf("proc. position: %02d:%06.3f\tprocessed: %02d:%06.3f\tplayed: %02d:%06.3f\t%5d samples ahead\t%5lu samples missed\r", proc_pos.min, proc_pos.sec, proc_time.min, proc_time.sec, play_time.min, play_time.sec, parser_obj.get_ahead_samples(), parser_obj.missed_samples);
	}
	printf("\nparser object stopped\n");

	parser_thread.join();

	return 0;
}