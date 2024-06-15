#include <rateconv.h>
#include <string.h>
#include <cmath>

#define _PI_ 3.14159265358979323846264338327950288

static double _blackman(double x) {
    return 0.42 - 0.5 * cos(2 * _PI_ * x) + 0.08 * cos(4 * _PI_ * x);
}

static double _sinc(double x) {
    return (x == 0.0) ? 1.0 : sin(_PI_ * x) / (_PI_ * x);
}

static double _windowed_sinc(int lw, double x) {
    return _blackman(0.5 + 0.5 * x / (lw / 2.0)) * _sinc(x);
}

rateconv::rateconv(int num_channels, float f_in, float f_out) : channels(num_channels), _f_ratio(f_in / f_out) {
    _lw = RATECONV_LW_MIN; while((float)_lw < _f_ratio) _lw += 2; // find suitable window size so that we can always have all new samples in our buffer

    _buf = new sample_t[num_channels * _lw];
    memset(_buf, 0, num_channels * _lw * sizeof(sample_t));
    
    /* populate sinc lookup table */
    _sinctab = new float[RATECONV_SINC_RESO * _lw / 2];
    for(int i = 0; i < RATECONV_SINC_RESO * _lw / 2; i++) {
        const double x = (double)i / RATECONV_SINC_RESO;
        if(44100.0 < f_in) {
            /* downsampling */
            _sinctab[i] = _windowed_sinc(_lw, x / _f_ratio) / _f_ratio;
        } else {
            _sinctab[i] = _windowed_sinc(_lw, x);
        }
    }
}

rateconv::~rateconv() {
    delete _buf;
}

void rateconv::put_sample(int channel, sample_t val) {
    sample_t* buf = &_buf[channel * _lw];
    memmove(buf, &buf[1], sizeof(sample_t) * (_lw - 1));
    buf[_lw - 1] = val;
}

static inline float lookup_sinc_table(const float* table, int lw, double x) {
    int idx = (int)(x * RATECONV_SINC_RESO);
    if(idx < 0) idx = -idx;
    if(idx > RATECONV_SINC_RESO * lw / 2 - 1) idx = RATECONV_SINC_RESO * lw / 2 - 1;
    return table[idx];
}

int rateconv::advance_timer() {
    _timer += _f_ratio;
    int num_clocks = floor(_timer);
    _timer -= num_clocks;
    return num_clocks;
}

sample_t rateconv::get_sample(int channel) {
    sample_t* buf = &_buf[channel * _lw];
    sample_t samp = 0;
    for(int k = 0; k < _lw; k++) {
        double x = ((double)k - (_lw / 2 - 1)) - _timer;
        samp += buf[k] * lookup_sinc_table(_sinctab, _lw, x);
    }
    return samp;
}