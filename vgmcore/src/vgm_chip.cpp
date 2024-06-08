#include <vgm_chip.h>

pff vgm_chip::mix_channels() {
    float left = 0, right = 0;
    size_t channels_count = _channels.size();
    for(size_t i = 0; i < channels_count; i++) {
        left += _channels[i] * _channels_pan[i].first;
        right += _channels[i] * _channels_pan[i].second;
    }
    return std::make_pair(left / channels_count, right / channels_count);
}