#include "player.h"

void player::sample_cb(const float* inbuf, float* outbuf, long frames, void* data) {
	player_cb_ctx_t* ctx = (player_cb_ctx_t*) data; // get context

	// printf("frames: %ld\n", frames);
	for(int i = 0; i < frames; i++) {
		stereo_sample_t sample = {0, 0};
        if(ctx->buf->try_dequeue(sample)) (*ctx->played_samples)++;
        else {
            (*ctx->missed_samples)++;
            // fprintf(stderr, "audio circular buffer underrun, playing silence\n");
        }
        
		*outbuf++ = sample.left;
		*outbuf++ = sample.right;
	}
}

player::player(sample_buf_t* buf, size_t rate) : _buf(buf), _pa(new Pa(sample_cb, 0, 2, rate, 0, &_ctx)), _ctx({_buf, &_played_samples, &_missed_samples}) {
    
}

player::player(size_t buf_size, size_t rate) : player(new sample_buf_t(buf_size), rate) {
    _newbuf = true;
}

player::~player() {
    delete _pa;
    if(_newbuf) delete _buf;
}

void player::put_sample(const stereo_sample_t& sample) {
    _buf->wait_enqueue(sample);
}

void player::start() {
    _pa->start();
}

void player::stop() {
    _pa->stop();
}