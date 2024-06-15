#include <vgm_types.h>

time_ms_t vgm_samp_to_minsec(size_t samples) {
	float dur_seconds = samples / 44100.0;
	int min = (int)dur_seconds / 60;
	float sec = dur_seconds - min * 60;
	return std::make_pair(min, sec);
}