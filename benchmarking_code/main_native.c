// Copyright 2010 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
//  Command-line tool for decoding a WebP image.
//
// Author: Skal (pascal.massimino@gmail.com)

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#include "decode_webp.h"
#include "helpers.h"


int main(int argc, const char* argv[]) {
  const char* in_file = NULL;
  const char* out_time_file = NULL;
  int iterations = 100;

  printf("Webp version 1.3.2\n");

  if (argc < 3) {
    printf("Usage: dwebp in_file [input_file] [output_time] [iterations]\n\n"
        "Decodes the WebP image file to YUV format [Default].\n"
        "Note: Animated WebP files are not supported.\n\n"
      );
    return 0;
  }

  in_file = argv[1];
  out_time_file = argv[2];
  if (argc == 4) {
    iterations = atoi(argv[3]);
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

  uint8_t* result = NULL;
  size_t result_size = 0;


  Stopwatch stop_watch;
  StopwatchReset(&stop_watch);
  int status = DecodeWebpImage(data, data_size, iterations, &result, &result_size);
  const double dt = StopwatchReadAndReset(&stop_watch);
  if (status != 0) {
    fprintf(stderr, "Failed to decode :(\n");
    return -1;
  }
  fprintf(stderr, "Time to decode %d pictures: %.10fs\n", iterations, dt);

  if (result_size > 0 && result != NULL) {
    fprintf(stderr, "Saving %zu bytes to native_out.ppm\n", result_size);
    FILE *outfile = fopen("native_out.ppm", "wb");
    if (!outfile) {
      fprintf(stderr, "Unable to open native_out.ppm\n");
      return -1;
    }

    fwrite(result, sizeof(uint8_t), result_size, outfile);
    fclose(outfile);
  }
  fprintf(out_time, "%f\n", dt);
  fclose(out_time);
  free((void*)data);
  free(result);
  return 0;
}
