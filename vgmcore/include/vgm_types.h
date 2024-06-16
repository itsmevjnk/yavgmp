#pragma once

#include <stddef.h>
#include <stdint.h>
#include <utility>

/* shorthands */
typedef std::pair<float, float> pff;
typedef std::pair<int, float> pif;

/* audio sample data */
typedef float sample_t;
typedef struct {
    sample_t left;
    sample_t right;
} stereo_sample_t;

/* pan/volume setting */
typedef float volume_t;
typedef struct {
    volume_t left;
    volume_t right;
} stereo_volume_t;

/* timestamp/duration */
typedef struct {
    int min;
    float sec;
} time_ms_t; // ms stands for min+sec
time_ms_t vgm_samp_to_minsec(size_t samples);
