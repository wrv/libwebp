#include "decode_webp_wasmsimd.h"
#include "helpers.h"

//#define OUTPUT_IMAGE

#ifdef OUTPUT_IMAGE
#include "uvwasi.h"
#include "uvwasi-rt.h"
#endif

int main(int argc, const char** argv) {
  const char* in_file = NULL;
  const char* out_time_file = NULL;
  const char* out_file_name = NULL;
  int iterations = 100;

  printf("WASMSIMD Webp version 1.3.2");

#ifdef OUTPUT_IMAGE
  printf(" (outputting image)\n");
#else
  printf(" (no output)\n");
#endif

  if (argc < 3) {
    print_usage();
    return 0;
  }

  in_file = argv[1];
  out_time_file = argv[2];

  if (argc > 3) {
    out_file_name = argv[3];
    if (argc > 4) {
      iterations = atoi(argv[4]);
    }
  }

  FILE *out_time = fopen(out_time_file, "a");
  if (!out_time) {
    printf("Unable to open %s\n", out_time_file);
    return -1;
  }

  const uint8_t* data = NULL;
  size_t data_size = 0;
  if (!open_file(in_file, &data, &data_size)) {
    return -1;
  }
  /* Initialize the Wasm runtime. */
  wasm_rt_init();
  /* Declare an instance of the `inst` module. */
  w2c_decode__webp__wasmsimd inst = { 0 };

#ifdef OUTPUT_IMAGE

  uvwasi_t local_uvwasi_state = {0};

  struct w2c_wasi__snapshot__preview1 wasi_env = {
    .uvwasi = &local_uvwasi_state,
    .instance_memory = &inst.w2c_memory
  };

  uvwasi_options_t init_options;
  uvwasi_options_init(&init_options);

  //pass in standard descriptors
  init_options.in = 0;
  init_options.out = 1;
  init_options.err = 2;
  init_options.fd_table_size = 3;


  //no sandboxing enforced, binary has access to everything user does
  init_options.preopenc = 2;
  init_options.preopens = calloc(2, sizeof(uvwasi_preopen_t));

  init_options.preopens[0].mapped_path = "/";
  init_options.preopens[0].real_path = "/";
  init_options.preopens[1].mapped_path = "./";
  init_options.preopens[1].real_path = ".";

  init_options.allocator = NULL;
  
  uvwasi_errno_t ret = uvwasi_init(&local_uvwasi_state, &init_options);

  if (ret != UVWASI_ESUCCESS) {
    printf("uvwasi_init failed with error %d\n", ret);
    exit(1);
  }

  /* Construct the module instance. */
  wasm2c_decode__webp__wasmsimd_instantiate(&inst, &wasi_env);
#else
  /* Construct the module instance. */
  wasm2c_decode__webp__wasmsimd_instantiate(&inst);
#endif

  wasm_rt_memory_t* mem = w2c_decode__webp__wasmsimd_memory(&inst);

  if (mem->size < data_size) {
    fprintf(stderr, "File too big (%zu) for WASM memory (%lu) \n", data_size, mem->size);
    goto EXIT;
  }
  // Allocate sandbox memory
  u32 webp_file = w2c_decode__webp__wasmsimd_malloc(&inst, data_size);
  // Copy data to sandbox memory
  memcpy(&(mem->data[webp_file]), data, data_size);

  /////////////////////////////////////////////////////////////////////

  // Result is a pointer to a pointer
  u32 result = w2c_decode__webp__wasmsimd_malloc(&inst, sizeof(u32));
  u32 result_size = w2c_decode__webp__wasmsimd_malloc(&inst, sizeof(u32));

  Stopwatch stop_watch;
  StopwatchReset(&stop_watch);
  w2c_decode__webp__wasmsimd_DecodeWebpImage(&inst, webp_file, data_size, iterations, result, result_size);
  const double dt = StopwatchReadAndReset(&stop_watch);
  fprintf(stderr, "Time to %d decode pictures: %.10fs\n", iterations, dt);
  fprintf(out_time, "%f\n", dt);

  u32 size = *(u32*) (&mem->data[result_size]);

  if (size > 0) {
    u32 output_ptr = *(u32*) (&mem->data[result]);
    if (!save_file(out_file_name, &(mem->data[output_ptr]), size)){
      return -1;
    }
  }

  /////////////////////////////////////////////////////////////////////
EXIT:
  fclose(out_time);
  free((void*)data);
  /* Free the inst module. */
  wasm2c_decode__webp__wasmsimd_free(&inst);
  /* Free the Wasm runtime state. */
  wasm_rt_free();
  return 0;
}