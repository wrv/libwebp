#include "decode_webp.h"

extern void* VP8GetCPUInfo;   // opaque forward declaration.

__attribute__((export_name("SetupWebpDecode")))
void SetupWebpDecode(const uint8_t* data, size_t data_size, void* returnconfig) {
  int ok = 0;
  WebPDecoderConfig config;
  WebPDecBuffer* const output_buffer = &config.output;
  WebPBitstreamFeatures* const bitstream = &config.input;

  WebPInitDecoderConfig(&config);

  VP8StatusCode status = VP8_STATUS_OK;
  status = WebPGetFeatures(data, data_size, bitstream);

  // Appropriate colorspace for YUV output stream
  output_buffer->colorspace = bitstream->has_alpha ? MODE_YUVA : MODE_YUV;
  returnconfig = &config;
}

__attribute__((export_name("DecodeWebpImage")))
int DecodeWebpImage(const uint8_t* data, size_t data_size, void *config) {
  int ok = 0;
  int iterations = 100;
  VP8StatusCode status;
  for(int i = 0; i < iterations; i++) {
    status = WebPDecode(data, data_size, (WebPDecoderConfig*) config);
  }

  if (status == VP8_STATUS_OK) ok = 1;

  return ok ? 0 : -1;
}

__attribute__((export_name("CleanupWebpDecode")))
void CleanupWebpDecode(void *config) {
    free(((WebPDecoderConfig*) config)->output.private_memory);
}