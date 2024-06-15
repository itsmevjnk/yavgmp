#include <pa.h>
#include <vgm_parser.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

/* chip emulators */
#include <psg_native.h>
#include <opll_wrapper.h>
#include <opl_wrapper.h>
#include <opn2_wrapper.h>
#include <emu2149_wrapper.h>
#include <scc_wrapper.h>
#include <rf5c.h>

int sample_cb(const void* in, void* out, ulong frames, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* data) {
	vgm_parser* vgm = (vgm_parser*) data;
	// printf("frames: %ld\n", frames);
	float* outbuf = (float*) out;
	for(int i = 0; i < frames; i++) {
		vgm->parse_until_next_sample();
		if(vgm->play_ended) {
			// printf("no more data\n");
			// cb_data->pa->stop(); // no more data
			return paComplete;
		} 
		stereo_sample_t parser_out = vgm->mix_outputs();
		*outbuf++ = parser_out.first;
		*outbuf++ = parser_out.second;
		
	}
	return paContinue;
}

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

	vgm_parser vgm(infile, false);
	psg_native::install(vgm);
	opll_wrapper::install(vgm);
	opl_wrapper::install(vgm);
	opn2_wrapper::install(vgm);
	emu2149_wrapper::install(vgm);
	scc_wrapper::install(vgm);
	rf5c::install(vgm);


	/* set up playing with PortAudio */
	Pa a(sample_cb, 0, 2, 44100, 0, &vgm);
	a.start();

	time_ms_t total_dur = vgm.header.get_total_duration();
	printf("playing VGM file: %s (%02d:%06.3f%s)\n", infile, total_dur.first, total_dur.second, vgm.header.fields.loop_samples ? ", looped" : "");
	while(1) {
		time_ms_t pos = vgm.get_track_position();
		printf("position: %02d:%06.3f\r", pos.first, pos.second);
	}

	return 0;
}