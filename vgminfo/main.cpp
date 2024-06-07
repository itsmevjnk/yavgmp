#include <fstream>
#include <vgm_file.h>

int main(int argc, char* argv[]) {
	if(argc != 2) {
		printf("vgminfo: invalid number of arguments given\n");
		return 1;
	}

	vgm_file vgm(argv[argc - 1]); // open file
	printf("%s : %s VGM file (%u bytes uncompressed)\n\n", argv[argc - 1], (vgm.compressed) ? "compressed" : "uncompressed", vgm.header.fields.eof_offset);

	printf("Version               : %u.%02u\n", vgm.header.fields.version_maj, vgm.header.fields.version_min);
	std::pair<int, float> total_dur = vgm.header.get_total_duration();
	printf("Total samples         : %u\t(%02u:%05.3f)\t@ 0x%x\n", vgm.header.fields.total_samples, total_dur.first, total_dur.second, vgm.header.fields.data_offset);
	std::pair<int, float> loop_dur = vgm.header.get_loop_duration();
	printf("Loop samples          : %u\t(%02u:%05.3f)\t@ 0x%x\n", vgm.header.fields.loop_samples, loop_dur.first, loop_dur.second, vgm.header.fields.loop_offset);
	printf("GD3 offset            : 0x%x\n", vgm.header.fields.gd3_offset);
	printf("Extra header offset   : 0x%x\n", vgm.header.fields.exthdr_offset);
	printf("Recording rate        : %u Hz\n", vgm.header.fields.rate);
	printf("Volume modifier       : 0x%02x\t(x%.2f)\n", vgm.header.fields.volume_mod, vgm.header.get_volume_factor());
	printf("Loop base             : %d\n", vgm.header.fields.loop_base);
	printf("Loop modifier         : 0x%02x\t(x%.4f)\n", vgm.header.fields.loop_mod, vgm.header.get_loop_multiplier());

	printf("PSG clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.psg_clock));
	printf("PSG feedback pattern  : 0x%04x\n", vgm.header.fields.psg_fb_pattern);
	printf("PSG LFSR width        : %u\n", vgm.header.fields.psg_sr_width);
	printf("PSG flags             : 0x%02x\n", vgm.header.fields.psg_flags);

	printf("OPN2 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opn2_clock));
	printf("OPM clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.opm_clock));

	printf("Sega PCM clock        : %u Hz\n", VGM_CLOCK(vgm.header.fields.spcm_clock));
	printf("Sega PCM I/F register : 0x%x\n", vgm.header.fields.spcm_if_reg);
	printf("RF5C68 clock          : %u Hz\n", VGM_CLOCK(vgm.header.fields.rf68_clock));

	printf("OPN clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.opn_clock));
	printf("OPN SSG flags         : 0x%02x\n", vgm.header.fields.opn_ssg_flags);

	printf("OPNA clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opna_clock));
	printf("OPNA SSG flags        : 0x%02x\n", vgm.header.fields.opna_ssg_flags);

	printf("OPNB clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opnb_clock));
	printf("OPL2 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opl2_clock));
	printf("OPL clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.opl_clock));
	printf("MSX-Audio clock       : %u Hz\n", VGM_CLOCK(vgm.header.fields.msx_clock));
	printf("OPL3 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opl3_clock));
	printf("OPL4 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.opl4_clock));
	printf("OPX clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.opx_clock));
	printf("PCMD8 clock           : %u Hz\n", VGM_CLOCK(vgm.header.fields.pcmd8_clock));
	printf("RF5C164 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.rf164_clock));
	printf("PWM clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.pwm_clock));

	printf("AY8910 clock          : %u Hz\n", VGM_CLOCK(vgm.header.fields.ay8910_clock));
	printf("AY8910 type           : 0x%02x\n", vgm.header.fields.ay8910_type);
	printf("AY8910 flags          : 0x%02x\n", vgm.header.fields.ay8910_flags);

	printf("DMG clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.dmg_clock));
	printf("APU clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.apu_clock));
	printf("MultiPCM clock        : %u Hz\n", VGM_CLOCK(vgm.header.fields.mpcm_clock));
	printf("uPD7759 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.pd59_clock));

	printf("OKIM6258 clock        : %u Hz\n", VGM_CLOCK(vgm.header.fields.ok58_clock));
	printf("OKIM6258 flags        : 0x%02x\n", vgm.header.fields.ok58_flags);

	printf("K054539 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.k39_clock));
	printf("K054539 flags         : 0x%02x\n", vgm.header.fields.k39_flags);

	printf("C140 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.c140_clock));
	printf("C140 type             : %u\n", vgm.header.fields.c140_type);

	printf("OKIM6295 clock        : %u Hz\n", VGM_CLOCK(vgm.header.fields.ok95_clock));

	printf("SCC clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.dmg_clock));
	printf("HuC6280 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.hu_clock));
	printf("K053260 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.k60_clock));
	printf("Pokey clock           : %u Hz\n", VGM_CLOCK(vgm.header.fields.pokey_clock));
	printf("QSound clock          : %u Hz\n", VGM_CLOCK(vgm.header.fields.qsound_clock));
	printf("SCSP clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.scsp_clock));
	printf("WonderSwan clock      : %u Hz\n", VGM_CLOCK(vgm.header.fields.wswan_clock));
	printf("VSU clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.vsu_clock));
	printf("SAA1099 clock         : %u Hz\n", VGM_CLOCK(vgm.header.fields.saa_clock));

	printf("DOC clock             : %u Hz\n", VGM_CLOCK(vgm.header.fields.doc_clock));
	printf("DOC int. channels     : %u\n", vgm.header.fields.doc_channels);

	printf("OTTO clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.otto_clock));
	printf("OTTO int. channels    : %u\n", vgm.header.fields.otto_channels);

	printf("C352 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.c352_clock));
	printf("C352 clock divider    : %u\n", vgm.header.get_c352_clkdiv());

	printf("X1-010 clock          : %u Hz\n", VGM_CLOCK(vgm.header.fields.x1_clock));
	printf("GA20 clock            : %u Hz\n", VGM_CLOCK(vgm.header.fields.ga20_clock));
	printf("Mikey clock           : %u Hz\n", VGM_CLOCK(vgm.header.fields.mikey_clock));

	return 0;
}