#include <vgm_chip_rateconv.h>
#include <cmath>
#include <stdio.h>

#define FREQ                        440 // sine wave frequency
#define DUR                         1.0 // sine wave duration (in sec)
#define F_IN                        7670453 // rate converter input sample rate
#define F_OUT                       44100 // rate converter output sample rate

#define SAMPLES                     (int)(F_OUT * DUR)

class rctest : public vgm_chip_rateconv {
public:
    rctest() : vgm_chip_rateconv(1, F_IN) {

    }

    void write(uintptr_t chip, uintptr_t port, uintptr_t addr_reg, uintptr_t val) {

    }

    void clock() {
        put_sample(0, sin(2 * 3.14 * FREQ * _timestamp));
        _timestamp += 1.0 / F_IN;
    }
private:
    double _timestamp = 0;
};

int main() {
    FILE* output = fopen("output.dat", "wb");
    rctest test;
    float min_amp = INFINITY, max_amp = -INFINITY;
    for(int i = 0; i < SAMPLES; i++) {
        test.next_sample();
        fwrite(&test.channels_out_left[0], sizeof(float), 1, output);
        if(test.channels_out_left[0] > max_amp) max_amp = test.channels_out_left[0];
        if(test.channels_out_left[0] < min_amp) min_amp = test.channels_out_left[0];
    }
    printf("%u samples written\nmin amplitude: %f\nmax amplitude: %f\n", SAMPLES, min_amp, max_amp);
    fclose(output);
    return 0;
}
