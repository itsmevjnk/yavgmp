#include <string.h>
#include <stdio.h>

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
	/* 0x14 */ 0x03, 0x00, // float32 format
	/* 0x16 */ 0x02, 0x00, // stereo = 2, mono = 1
	/* 0x18 */ 0x44, 0xAC, 0x00, 0x00, // sample rate (44100 Hz)
	/* 0x1C */ 0x20, 0x62, 0x05, 0x00, // bytes to read per second (44100 * 4 * 1 = 10 B1 02 00 if mono, or 44100 * 4 * 2 = 20 62 05 00 if stereo)
	/* 0x20 */ 0x08, 0x00, // bytes per block (4 if mono, or 8 if stereo)
	/* 0x22 */ 0x20, 0x00, // bits per sample (32)

	/* data chunk */
	/* 0x24 */ 'd', 'a', 't', 'a',
	/* 0x28 */ 0x00, 0x00, 0x00, 0x00 // data size (4 * samples)
};

FILE** outputs[sizeof(chips) / sizeof(chips[0])]; // list of arrays to file handlers (one file per channel)

#define ENDIAN_FLIP_32(x)					((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

/* actual dump function */
bool new_sample_handler(vgm_parser* parser) {
	for(int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
		if(parser->chips_array[i]) {
			/* dump samples from chip */
			int num_channels = parser->chips_array[i]->channels;
			
			float samp_float; uint32_t* samp = (uint32_t*)&samp_float;
			for(int j = 0; j < num_channels; j++) {
				if(outputs[i][j]) {
					samp_float = parser->chips_array[i]->channels_out_left[j] * parser->chips_array[i]->channels_pan[j].first;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
					*samp = ENDIAN_FLIP_32(*samp); // convert to little endian
#endif
					fwrite(samp, 4, 1, outputs[i][j]);
					samp_float = parser->chips_array[i]->channels_out_right[j] * parser->chips_array[i]->channels_pan[j].second; 
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
					*samp = ENDIAN_FLIP_32(*samp);
#endif						
					fwrite(samp, 4, 1, outputs[i][j]);
				}
			}

			if(outputs[i][num_channels]) {
				pff out = parser->chips_array[i]->mix_channels();
				samp_float = out.first;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
				*samp = ENDIAN_FLIP_32(*samp); // convert to little endian
#endif
				fwrite(samp, 4, 1, outputs[i][num_channels]);
				samp_float = out.second;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
				*samp = ENDIAN_FLIP_32(*samp);
#endif						
				fwrite(samp, 4, 1, outputs[i][num_channels]);
			}
		}
	}

	return true; // continue playing
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		printf("vgmcdump: invalid number of arguments given\n");
		return 1;
	}

	vgm_parser vgm(argv[argc - 1], false); // open file

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
	uint32_t data_size = 4 * section_samples * 2;
	uint32_t file_size = data_size + 32;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	data_size = ENDIAN_FLIP_32(data_size);
	file_size = ENDIAN_FLIP_32(file_size);
#endif
	*((uint32_t*)&wav_header[0x04]) = file_size;
	*((uint32_t*)&wav_header[0x28]) = data_size;

	/* open output files */
	for(int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
		if(vgm.chips_array[i]) {
			/* chip exists - create channel outputs for it */
			int num_channels = vgm.chips_array[i]->channels;
			if(!outputs[i]) {
				outputs[i] = new FILE*[num_channels + 1]; // NOTE: we don't need to delete since the program will exit anyway
			}
			memset(outputs[i], 0, (num_channels + 1) * sizeof(FILE*)); // to be sure
			char path_buf[257];
			for(int j = 0; j < num_channels; j++) {
				snprintf(path_buf, 257, "%s_%d_%s_%d.wav", argv[argc - 1], section, chips[i], j);
				outputs[i][j] = fopen(path_buf, "wb");
				if(!outputs[i][j]) printf("cannot open %s\n", path_buf);
				else {
					printf("sec#%d: %s channel %d will be dumped to %s\n", section, chips[i], j, path_buf);
					fwrite(wav_header, sizeof(wav_header), 1, outputs[i][j]); // write header
				}
			}
			snprintf(path_buf, 257, "%s_%d_%s_mixed.wav", argv[argc - 1], section, chips[i]);
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
		if(!vgm.parse_next()) break; // premature end(?)
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

	if(!vgm.play_ended && !vgm.played_loops && vgm.header.fields.loop_samples) {
		if(section) return 0; // done
		section++;
		section_samples = vgm.header.fields.loop_samples; // we'll play the loop next
		goto play_section;
	}

	return 0;
}