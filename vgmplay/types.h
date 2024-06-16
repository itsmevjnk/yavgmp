/* data types used around vgmplay */

#pragma once

#include <vgm_types.h>
#include <readerwritercircularbuffer.h>

typedef moodycamel::BlockingReaderWriterCircularBuffer<stereo_sample_t> sample_buf_t; // sample circular buffer
