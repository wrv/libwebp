#include <stdio.h>
#include <stdlib.h>

#include "decode_webp_wasmsimd.h"
#include "uvwasi.h"
#include "uvwasi-rt.h"


int load_data(const char* const in_file,
             const uint8_t** data, size_t* data_size) {
  int ok;
  uint8_t* file_data;
  size_t file_size;
  FILE* in;

  *data = NULL;
  *data_size = 0;

  in = fopen(in_file, "rb");
  if (in == NULL) {
    fprintf(stderr, "cannot open input file '%s'\n", (const char*)in_file);
    return 0;
  }
  fseek(in, 0, SEEK_END);
  file_size = ftell(in);
  fseek(in, 0, SEEK_SET);
  // we allocate one extra byte for the \0 terminator
  file_data = (uint8_t*)malloc(file_size + 1);
  if (file_data == NULL) {
    fclose(in);
    fprintf(stderr, "memory allocation failure when reading file %s\n",
             (const char*)in_file);
    return 0;
  }
  ok = (fread(file_data, file_size, 1, in) == 1);
  fclose(in);

  if (!ok) {
    fprintf(stderr, "Could not read %d bytes of data from file %s\n",
             (int)file_size, (const char*)in_file);
    free(file_data);
    return 0;
  }
  file_data[file_size] = '\0';  // convenient 0-terminator
  *data = file_data;
  *data_size = file_size;

  return 1;
}

int main(int argc, const char** argv) {
  const char* in_file = NULL;
  const char* out_file = NULL;
  const char* out_time_file = NULL;

  printf("WASMSIMD Webp version 1.2.2\n");

  if (argc < 4) {
    printf("Usage: dwebp in_file [input_file] [output_file] [output_time]\n\n"
        "Decodes the WebP image file to YUV format [Default].\n"
        "Note: Animated WebP files are not supported.\n\n"
      );
    return 0;
  }

  in_file = argv[1];
  out_file = argv[2];
  out_time_file = argv[3];

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
  w2c_decode__webp__wasmsimd inst = { 0 };

  uvwasi_t local_uvwasi_state = {0};
  struct w2c_wasi__snapshot__preview1 wasi_env = {
    .uvwasi = &local_uvwasi_state,
    .instance_memory = &inst.w2c_memory
  };

  uvwasi_options_t init_options;
  uvwasi_options_init(&init_options);
  uvwasi_errno_t ret = uvwasi_init(&local_uvwasi_state, &init_options);
  if (ret != UVWASI_ESUCCESS) {
    printf("uvwasi_init failed with error %d\n", ret);
    exit(1);
  }
  /* Construct the module instance. */
  wasm2c_decode__webp__wasmsimd_instantiate(&inst, &wasi_env);

  wasm_rt_memory_t* mem = w2c_decode__webp__wasmsimd_memory(&inst);

  if (mem->size < data_size) {
    fprintf(stderr, "File too big (%zu) for WASM memory (%lu) \n", data_size, mem->size);
    goto EXIT;
  }
  // Allocate sandbox memory
  u32 webp_file = w2c_decode__webp__wasmsimd_malloc(&inst, data_size);
  // Copy data to sandbox memory
  memcpy(&(mem->data[webp_file]), data, data_size);

  // Call decode
  double dt = 0.0;
  dt = w2c_decode__webp__wasmsimd_DecodeWebpImage(&inst, webp_file, data_size);

  fprintf(stderr, "Time to decode pictures: %.10fs\n", dt);
  fprintf(out_time, "%f\n", dt);

EXIT:
  fclose(out_time);
  free((void*)data);
  /* Free the inst module. */
  wasm2c_decode__webp__wasmsimd_free(&inst);
  uvwasi_destroy(&local_uvwasi_state);
  /* Free the Wasm runtime state. */
  wasm_rt_free();
  return 0;
}