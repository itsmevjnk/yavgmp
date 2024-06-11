#include <vgm_chip_rateconv.h>
#include <string.h>
#include <cmath>

#define _PI_ 3.14159265358979323846264338327950288

#define SINCTAB_AMP                             (1.0 / 0.091879) // amplitude to amplify the sinc table up to (so that input data of range -1.0 .. 1.0 gets transformed to -1.0 .. 1.0)

static double _blackman(double x) {
    return 0.42 - 0.5 * cos(2 * _PI_ * x) + 0.08 * cos(4 * _PI_ * x);
}

static double _sinc(double x) {
    return (x == 0.0) ? 1.0 : sin(_PI_ * x) / (_PI_ * x);
}

static double _windowed_sinc(double x) {
    return _blackman(0.5 + 0.5 * x / (RATECONV_LW / 2)) * _sinc(x);
}

void vgm_chip_rateconv::reset_rateconv() {
    memset(_buf, 0, sizeof(float) * channels * RATECONV_LW);
}

vgm_chip_rateconv::vgm_chip_rateconv(size_t num_channels, size_t f_in) : _f_ratio(f_in / 44100.0), vgm_chip(num_channels) {
    _buf = new float[num_channels * RATECONV_LW];
    reset_rateconv();

    /* create sinc table */
    for(int i = 0; i < RATECONV_SINC_RESO * RATECONV_LW / 2; i++) {
        const double x = (double)i / RATECONV_SINC_RESO;
        if(44100.0 < f_in) {
            /* downsampling */
            _sinctab[i] = _windowed_sinc(x / _f_ratio) / _f_ratio;
        } else {
            _sinctab[i] = _windowed_sinc(x);
        }
        _sinctab[i] *= SINCTAB_AMP;
    }
}

vgm_chip_rateconv::~vgm_chip_rateconv() {
    delete _buf;
}

static inline float lookup_sinc_table(const float* table, double x) {
    int idx = (int)(x * RATECONV_SINC_RESO);
    if(idx < 0) idx = -idx;
    if(idx > RATECONV_SINC_RESO * RATECONV_LW / 2 - 1) idx = RATECONV_SINC_RESO * RATECONV_LW / 2 - 1;
    return table[idx];
}

void vgm_chip_rateconv::put_sample(size_t channel, float val) {
    if(channel >= channels) return;
    float* buf = &_buf[channel * RATECONV_LW];
    memmove(buf, &buf[1], sizeof(float) * (RATECONV_LW - 1));
    buf[RATECONV_LW - 1] = val;
}

void vgm_chip_rateconv::next_sample() {
    _timer += _f_ratio;
    int num_clocks = floor(_timer);
    double dn = _timer - num_clocks;
    _timer = dn;

    for(int i = 0; i < num_clocks; i++) clock();

    for(int i = 0; i < channels; i++) {
        float* buf = &_buf[i * RATECONV_LW];
        _channels_out[i] = 0;
        for(int k = 0; k < RATECONV_LW; k++) {
            double x = ((double)k - (RATECONV_LW / 2 - 1)) - dn;
            _channels_out[i] += buf[k] * lookup_sinc_table(_sinctab, x);
        }
    }
}
