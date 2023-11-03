#include "decode_webp.h"

typedef struct timeval Stopwatch;

static inline void StopwatchReset(Stopwatch* watch) {
  gettimeofday(watch, NULL);
}

static inline double StopwatchReadAndReset(Stopwatch* watch) {
  struct timeval old_value;
  double delta_sec, delta_usec;
  memcpy(&old_value, watch, sizeof(old_value));
  gettimeofday(watch, NULL);
  delta_sec = (double)watch->tv_sec - old_value.tv_sec;
  delta_usec = (double)watch->tv_usec - old_value.tv_usec;
  return delta_sec + delta_usec / 1000000.0;
}

extern void* VP8GetCPUInfo;   // opaque forward declaration.


__attribute__((export_name("DecodeWebpImage")))
double DecodeWebpImage(const uint8_t* data, size_t data_size) {
  int ok = 0;
  WebPDecoderConfig config;
  WebPDecBuffer* const output_buffer = &config.output;
  WebPBitstreamFeatures* const bitstream = &config.input;

  WebPInitDecoderConfig(&config);

  VP8StatusCode status = VP8_STATUS_OK;
  status = WebPGetFeatures(data, data_size, bitstream);
  if (status != VP8_STATUS_OK) {
    return -1;
  }

  // Appropriate colorspace for YUV output stream
  output_buffer->colorspace = bitstream->has_alpha ? MODE_YUVA : MODE_YUV;

  int iterations = 100;

  Stopwatch stop_watch;
  StopwatchReset(&stop_watch);
  for(int i = 0; i < iterations; i++) {
    status = WebPDecode(data, data_size, &config);
  }
  const double decode_time = StopwatchReadAndReset(&stop_watch);

  ok = (status == VP8_STATUS_OK);

  free(output_buffer->private_memory);

  return ok ? decode_time : -1;
}