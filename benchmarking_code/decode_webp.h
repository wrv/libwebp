#include <stdlib.h>
#include <string.h>
#include <webp/decode.h>
#include <webp/types.h>

void SetupWebpDecode(const uint8_t* data, size_t data_size, void *returnconfig);
int DecodeWebpImage(const uint8_t* data, size_t data_size, void *config);
void CleanupWebpDecode(void *config);