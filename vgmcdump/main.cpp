#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <vgm_parser.h>

#define PSG_USE_NATIVE // uncomment to use native PSG emulator implementation instead of emu76489 wrapper
#ifdef PSG_USE_NATIVE
#include <psg_native.h>
#else
#include <psg_wrapper.h>
#endif

#include <opll_wrapper.h>
#include <opl_wrapper.h>
#include <opn2_wrapper.h>
#include <emu2149_wrapper.h>
#include <scc_wrapper.h>
#include <rf5c.h>

/* chip names according to their position in vgm_parser::chips */
const char* chips[] = {
	"psg",
	"opll",
	"opn2",
	"opm",
	"spcm",
	"rf68",
	"opn",
	"opna",
	"opnb",
	"opl2",
	"opl",
	"msx",
	"opl3",
	"opl4",
	"opx",
	"pcmd8",
	"rf164",
	"pwm",
	"ay8910",
	"dmg",
	"apu",
	"mpcm",
	"pd59",
	"ok58",
	"ok95",
	"scc",
	"k39",
	"hu",
	"c140",
	"k60",
	"pokey",
	"qsound",
	"scsp",
	"wswan",
	"vsu",
	"saa",
	"doc",
	"otto",
	"c352",
	"x1",
	"ga20",
	"mikey",
};

uint8_t wav_header[] = {
	/* 0x00 */ 'R', 'I', 'F', 'F',
	/* 0x04 */ 0x00, 0x00, 0x00, 0x00, // file size minus 8 bytes (LE) - i.e. data size + 4 ("data") + 16 (data format contents) + 8 ("fmt " + chunk size) + 4 ("WAVE") = data size + 32
	/* 0x08 */ 'W', 'A', 'V', 'E',

	/* data format chunk */
	/* 0x0C */ 'f', 'm', 't', ' ',
	/* 0x10 */ 0x10, 0x00, 0x00, 0x00, // chunk size minus 8 (16)
	/* 0x14 */ 0x00, 0x00, // 1 = signed 16-bit integer, 3 = 32-bit float
	/* 0x16 */ 0x02, 0x00, // stereo = 2, mono = 1
	/* 0x18 */ 0x44, 0xAC, 0x00, 0x00, // sample rate (44100 Hz)
	/* 0x1C */ 0x00, 0x00, 0x00, 0x00, // bytes to read per second (44100 * 4 * 1 = 10 B1 02 00 if mono, or 44100 * 4 * 2 = 20 62 05 00 if stereo)
	/* 0x20 */ 0x00, 0x00, // bytes per block (2/4 if mono, or 4/8 if stereo)
	/* 0x22 */ 0x00, 0x00, // bits per sample (16 if integer, or 32 if float)

	/* data chunk */
	/* 0x24 */ 'd', 'a', 't', 'a',
	/* 0x28 */ 0x00, 0x00, 0x00, 0x00 // data size (4 * samples)
};

FILE* mixed_output; // final mixed output file
FILE** outputs[sizeof(chips) / sizeof(chips[0])]; // list of arrays to file handlers (one file per channel)

#define ENDIAN_FLIP_16(x)					((((x) & 0x00FF) << 8) | (((x) & 0xFF00) >> 8))
#define ENDIAN_FLIP_32(x)					((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

bool wav_float = true;
void write_sample(sample_t samp, FILE* fp) {
	if(wav_float) {
		/* write sample as-is */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN_
		uint32_t* samp_int = *((uint32_t*)&samp);
		*samp_int = ENDIAN_FLIP_32(*samp_int); // convert to little endian
#endif
		fwrite(&samp, 4, 1, fp);		
	} else {
		/* convert to integer first */
		if(samp > 1.0) samp = 1.0; else if(samp < -1.0) samp = -1.0; // clamp sample to -1.0..1.0
		int16_t samp_int = samp * 32767;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN_
		samp_int = ENDIAN_FLIP_16(samp_int); // convert to little endian
#endif		
		fwrite(&samp_int, 2, 1, fp);
	}
}

/* actual dump function */
bool new_sample_handler(vgm_parser* parser) {
	for(int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
		if(parser->chips_array[i]) {
			/* dump samples from chip */
			int num_channels = parser->chips_array[i]->channels;
			
			for(int j = 0; j < num_channels; j++) {
				if(outputs[i][j]) {
					write_sample(parser->chips_array[i]->channels_out_left[j] * parser->chips_array[i]->channels_pan[j].left, outputs[i][j]);
					write_sample(parser->chips_array[i]->channels_out_right[j] * parser->chips_array[i]->channels_pan[j].right, outputs[i][j]);
				}
			}

			if(outputs[i][num_channels]) {
				stereo_sample_t out = parser->chips_array[i]->mix_channels();
				write_sample(out.left, outputs[i][num_channels]);
				write_sample(out.right, outputs[i][num_channels]);
			}
		}
	}

	if(mixed_output) {
		stereo_sample_t out = parser->mix_outputs();
		write_sample(out.left, mixed_output);
		write_sample(out.right, mixed_output);
	}

	return true; // continue playing
}

char output_prefix[257]; // output file path prefix (defaults to input path)

int main(int argc, char* argv[]) {
	opterr = 0;
	int c;
	output_prefix[0] = '\0'; // make it an empty string
	while((c = getopt(argc, argv, "iho:")) != -1) {
		switch(c) {
			case 'i':
				wav_float = false;
				break;
			case 'h':
				printf("vgmcdump - dump channel/chip outputs from VGM file\n");
				printf("usage: vgmcdump [-i] [-o PREFIX] VGM_FILE\n");
				printf("optional arguments:\n");
				printf("\t-i\t\toutput 16-bit signed integer WAV files instead of 32-bit floating point\n");
				printf("\t-o PREFIX\tuse PREFIX as prefix for output file paths\n");
				return 0;
			case 'o':
				strncpy(output_prefix, optarg, 257);
				break;
			case '?':
				if(optopt == 'o')
					fprintf(stderr, "vgmcdump: option -%c requires an argument\n", optopt);
				else if(isprint(optopt))
					fprintf(stderr, "vgmcdump: unknown option -%c\n", optopt);
				else
					fprintf(stderr, "vgmcdump: unknown character 0x%x\n", optopt);
				return 1;
			default:
				abort();
		}		
	}

	if(optind == argc) {
		fprintf(stderr, "vgmcdump: no input file given\n");
		return 1;
	}
	const char* infile = argv[optind];
	if(!output_prefix[0]) strncpy(output_prefix, infile, 257);
	output_prefix[256] = '\0';

	vgm_parser vgm(infile, false); // open file

	/* install chip emulators */
#ifdef PSG_USE_NATIVE
	psg_native::install(vgm);
#else
	psg_wrapper::install(vgm);
#endif
	opll_wrapper::install(vgm);
	opl_wrapper::install(vgm);
	opn2_wrapper::install(vgm);
	emu2149_wrapper::install(vgm);
	scc_wrapper::install(vgm);
	rf5c::install(vgm);

	vgm.on_new_sample = &new_sample_handler; // set sample handler

	int section = 0;
	uint32_t section_samples = vgm.header.fields.total_samples - vgm.header.fields.loop_samples; // number of samples to be played

	memset(outputs, 0, sizeof(outputs)); // clear out our outputs list

play_section:
	/* calculate fields to be put into the WAV header */
	uint32_t data_size = (wav_float ? 4 : 2) * section_samples * 2;
	uint32_t file_size = data_size + 32;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	data_size = ENDIAN_FLIP_32(data_size);
	file_size = ENDIAN_FLIP_32(file_size);
#endif
	wav_header[0x14] = wav_float ? 3 : 1;
	wav_header[0x20] = wav_float ? 8 : 4;
	wav_header[0x22] = wav_float ? 32 : 16;
	*((uint32_t*)&wav_header[0x04]) = file_size;
	*((uint32_t*)&wav_header[0x28]) = data_size;
	uint32_t bytes_per_second = wav_header[0x20] * 44100;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	bytes_per_second = ENDIAN_FLIP_32(bytes_per_second);
#endif
	*((uint32_t*)&wav_header[0x1C]) = bytes_per_second;

	/* open output files */
	char path_buf[257];
	snprintf(path_buf, 257, "%s_%d_mixed.wav", output_prefix, section);
	mixed_output = fopen(path_buf, "wb");
	if(!mixed_output) printf("cannot open %s\n", path_buf);
	else {
		printf("sec#%d: final mixed output will be dumped to %s\n", section, path_buf);
		fwrite(wav_header, sizeof(wav_header), 1, mixed_output); // write header
	}

	for(int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
		if(vgm.chips_array[i]) {
			/* chip exists - create channel outputs for it */
			int num_channels = vgm.chips_array[i]->channels;
			if(!outputs[i]) {
				outputs[i] = new FILE*[num_channels + 1]; // NOTE: we don't need to delete since the program will exit anyway
			}
			memset(outputs[i], 0, (num_channels + 1) * sizeof(FILE*)); // to be sure
			for(int j = 0; j < num_channels; j++) {
				snprintf(path_buf, 257, "%s_%d_%s_%d.wav", output_prefix, section, chips[i], j);
				outputs[i][j] = fopen(path_buf, "wb");
				if(!outputs[i][j]) printf("cannot open %s\n", path_buf);
				else {
					printf("sec#%d: %s channel %d will be dumped to %s\n", section, chips[i], j, path_buf);
					fwrite(wav_header, sizeof(wav_header), 1, outputs[i][j]); // write header
				}
			}
			snprintf(path_buf, 257, "%s_%d_%s_mixed.wav", output_prefix, section, chips[i]);
			outputs[i][num_channels] = fopen(path_buf, "wb");
			if(!outputs[i][num_channels]) printf("cannot open %s\n", path_buf);
			else {
				printf("sec#%d: %s mixed output will be dumped to %s\n", section, chips[i], path_buf);
				fwrite(wav_header, sizeof(wav_header), 1, outputs[i][num_channels]); // write header
			}
		}
	}

	/* go on playing */
	size_t start_pos = vgm.played_samples;
	while(vgm.played_samples - start_pos < section_samples) {
		// printf("playing sample %lu/%u\n", vgm.played_samples, vgm.header.fields.total_samples);
		if(!vgm.parse_until_next_sample()) break; // premature end(?)
	}

	/* wrap up */
	for(int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
		if(outputs[i]) {
			size_t num_channels = vgm.chips_array[i]->channels;
			for(int j = 0; j <= num_channels; j++) {
				if(outputs[i][j]) fclose(outputs[i][j]);
			}
		}
	}
	if(mixed_output) fclose(mixed_output);

	if(!vgm.play_ended && !vgm.played_loops && vgm.header.fields.loop_samples) {
		if(section) return 0; // done
		section++;
		section_samples = vgm.header.fields.loop_samples; // we'll play the loop next
		goto play_section;
	}

	return 0;
}