/* audio sample player */

#pragma once

#include "types.h"
#include <pa.h>

/* callback context */
typedef struct {
    sample_buf_t* buf; // circular buffer
    size_t* played_samples;
    size_t* missed_samples;
} player_cb_ctx_t;

#define PLAYER_SAMPLE_BUF_DEFAULT_SIZE                  4410 // default sample buffer size

class player {
public:
    player(size_t buf_size = 4410, size_t rate = 44100); // initialise with new buffer
    player(sample_buf_t* buf, size_t rate = 44100); // initialise with existing buffer
    ~player();

    const size_t& played_samples = _played_samples; // number of samples played
    const size_t& missed_samples = _missed_samples; // number of missed samples (due to buffer underrun)

    void put_sample(const stereo_sample_t& sample); // put sample into circular buffer (blocking - will block if the buffer is full)

    void start();
    void stop();
private:
    sample_buf_t* _buf; // the circular buffer this will play from
    bool _newbuf = false; // set if _buf was instantiated by us (so we're responsible for disposing of it)

    size_t _played_samples = 0, _missed_samples = 0;
    Pa* _pa; // portaudio stream

    player_cb_ctx_t _ctx; // context for passing into sample callback

    static void sample_cb(const float* inbuf, float* outbuf, long frames, void* data);
};
