#include "decode_webp.h"

// Uncommenting this out saves the image to out.ppm
//#define OUTPUTIMAGE

#ifdef OUTPUTIMAGE
#include <stdio.h>
#endif

extern void* VP8GetCPUInfo;   // opaque forward declaration.

int SetupWebpDecode(const uint8_t* data, size_t data_size, const void** config) {
  int ok = 0;
  WebPDecoderConfig new_config;
  WebPDecBuffer* const output_buffer = &new_config.output;
  WebPBitstreamFeatures* const bitstream = &new_config.input;

  WebPInitDecoderConfig(&new_config);

  VP8StatusCode status = VP8_STATUS_OK;
  status = WebPGetFeatures(data, data_size, bitstream);
  
  if (status != VP8_STATUS_OK) {
    return -1;
  }
  // Appropriate colorspace for PPM output stream
  output_buffer->colorspace = MODE_RGB;
  *config = &new_config;

  return 0;
}

int DecodeWebpImage(const uint8_t* data, size_t data_size, const void* c, int iterations, uint8_t** result, size_t* result_size) {
  int ok = 0;
  VP8StatusCode status;
  WebPDecoderConfig config = *((WebPDecoderConfig *) c);

  for(int i = 0; i < iterations; i++) {
    status = WebPDecode(data, data_size, &config);
  }

  if (status == VP8_STATUS_OK) ok = 1;

#ifdef OUTPUTIMAGE
  const WebPDecBuffer* const buffer = &config.output;

  const uint32_t width = buffer->width;
  const uint32_t height = buffer->height;
  const uint8_t* row = buffer->u.RGBA.rgba;
  const int stride = buffer->u.RGBA.stride;
  const size_t bytes_per_px = 3;
  uint32_t y;

  char* header = malloc(30);
  if (header == NULL) return -1;
  memset(header, 0, 30);
  snprintf(header, 30, "P6\n%u %u\n255\n", width, height);
  size_t header_size = strlen(header);

  // Allocate the size of the result
  *result_size = height*width*bytes_per_px + header_size;
  *result = malloc(*result_size);

  if (row == NULL) return -1;
  if (*result == NULL) return -1;

  memcpy(*result, header, header_size);

  for (y = 0; y < height; ++y) {
    memcpy(&(*result)[header_size + y*stride], row, stride);
    row += stride;
  }
#endif

  return ok ? 0 : -1;
}

void CleanupWebpDecode(const void *config) {
  if (config != NULL) {
    WebPDecBuffer* const output_buffer = &((WebPDecoderConfig*) config)->output;
    WebPFreeDecBuffer(output_buffer);
  } 
}