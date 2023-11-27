#ifndef DECODE_WEBP_H_
#define DECODE_WEBP_H_

#include <stdlib.h>
#include <string.h>
#include <webp/decode.h>
#include <webp/types.h>

int SetupWebpDecode(const uint8_t* data, size_t data_size, const void **returnconfig);
int DecodeWebpImage(const uint8_t* data, size_t data_size, const void *config, int iterations, uint8_t** result, size_t* result_size);
void CleanupWebpDecode(const void *config);

#endif /* DECODE_WEBP_H_ */