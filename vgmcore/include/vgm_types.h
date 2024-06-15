#pragma once

#include <stddef.h>
#include <stdint.h>
#include <utility>

/* shorthands */
typedef std::pair<float, float> pff;
typedef std::pair<int, float> pif;

/* audio sample data */
typedef float sample_t;
typedef std::pair<sample_t, sample_t> stereo_sample_t;

/* pan/volume setting */
typedef float volume_t;
typedef std::pair<volume_t, volume_t> stereo_volume_t;

/* timestamp/duration */
typedef std::pair<int, float> time_ms_t; // ms stands for min+sec
time_ms_t vgm_samp_to_minsec(size_t samples);
