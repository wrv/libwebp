#include "decode_webp_wasmsimd_neon.h"
#include "helpers.h"

int main(int argc, const char** argv) {
  const char* in_file = NULL;
  const char* out_time_file = NULL;

  printf("WASMSIMD Webp version 1.2.2\n");

  if (argc < 3) {
    printf("Usage: dwebp in_file [input_file] [output_time file]\n\n"
        "Decodes the WebP image file to YUV format [Default].\n"
        "Note: Animated WebP files are not supported.\n\n"
      );
    return 0;
  }

  in_file = argv[1];
  out_time_file = argv[2];

  FILE *out_time = fopen(out_time_file, "a");
  if (!out_time) {
    printf("Unable to open %s\n", out_time_file);
    return -1;
  }

  const uint8_t* data = NULL;
  size_t data_size = 0;
  if (!load_data(in_file, &data, &data_size)) {
    return -1;
  }
  /* Initialize the Wasm runtime. */
  wasm_rt_init();
  /* Declare an instance of the `inst` module. */
  w2c_decode__webp__wasmsimd__neon inst = { 0 };

  /* Construct the module instance. */
  wasm2c_decode__webp__wasmsimd__neon_instantiate(&inst);

  wasm_rt_memory_t* mem = w2c_decode__webp__wasmsimd__neon_memory(&inst);

  if (mem->size < data_size) {
    fprintf(stderr, "File too big (%zu) for WASM memory (%lu) \n", data_size, mem->size);
    goto EXIT;
  }
  // Allocate sandbox memory
  u32 webp_file = w2c_decode__webp__wasmsimd__neon_malloc(&inst, data_size);
  // Copy data to sandbox memory
  memcpy(&(mem->data[webp_file]), data, data_size);

  /////////////////////////////////////////////////////////////////////
  // Get an address to which config can be set
  u32 config = w2c_decode__webp__wasmsimd__neon_malloc(&inst, data_size);

  w2c_decode__webp__wasmsimd__neon_SetupWebpDecode(&inst, webp_file, data_size, config);

  if (config == 0) {
    printf("Failed to initialize WebpDecoder\n");
    return -1;
  }

  Stopwatch stop_watch;
  StopwatchReset(&stop_watch);
  w2c_decode__webp__wasmsimd__neon_DecodeWebpImage(&inst, webp_file, data_size, config);
  const double dt = StopwatchReadAndReset(&stop_watch);
  fprintf(stderr, "Time to decode pictures: %.10fs\n", dt);
  fprintf(out_time, "%f\n", dt);

  w2c_decode__webp__wasmsimd__neon_CleanupWebpDecode(&inst, config);
  /////////////////////////////////////////////////////////////////////
EXIT:
  fclose(out_time);
  free((void*)data);
  /* Free the inst module. */
  wasm2c_decode__webp__wasmsimd__neon_free(&inst);
  /* Free the Wasm runtime state. */
  wasm_rt_free();
  return 0;
}