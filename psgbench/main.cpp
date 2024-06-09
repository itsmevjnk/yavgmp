#include <psg_native.h>
#include <psg_wrapper.h>

#include <time.h>

#define NUM_SAMPLES								44100 * 8

void do_test(vgm_chip* chip) {
	/* load data into chip so it has something to do */
	chip->write(0, 1, 0x06, 0xFF); // GG stereo write (because why not)
	uint8_t bytes[] = {0x8C, 0x06, 0xAE, 0x35, 0xC1, 0x00, 0xE4, 0x92, 0xB4, 0xD5, 0xF6}; // first note in Japanese SMS BIOS music
	for(int i = 0; i < sizeof(bytes); i++) chip->write(0, 0, 0, bytes[i]);

	/* make the chip tick for a while */
	struct timespec spec_1; clock_gettime(CLOCK_REALTIME, &spec_1); // start
	for(int i = 0; i < NUM_SAMPLES; i++) chip->next_sample();
	struct timespec spec_2; clock_gettime(CLOCK_REALTIME, &spec_2); // finish

	/* calculate elapsed time */
	long t_sec = spec_2.tv_sec - spec_1.tv_sec;
	long t_nsec = spec_2.tv_nsec - spec_1.tv_nsec;
	if(t_nsec < 0) t_nsec = 1000000000 - t_nsec;
	double t = t_sec + t_nsec / 1000000000.0;
	printf("It took %.9f seconds to emulate %d samples. The average speed is %.2f samples per second.\n", t, NUM_SAMPLES, (NUM_SAMPLES / t));
}

int main(int argc, char* argv[]) {
	/* create header containing PSG params */
	vgm_header_t header;
	header.psg_clock = 3579540;
	header.psg_fb_pattern = 0x0009;
	header.psg_flags = 0;
	header.psg_sr_width = 16;

	/* test with wrapper */
	printf("psg_wrapper: ");
	vgm_chip* chip = new psg_wrapper(header);
	do_test(chip);
	delete chip;

	/* test with native implementation */
	printf("psg_native: ");
	chip = new psg_native(header);
	do_test(chip);
	delete chip;

	return 0;
}