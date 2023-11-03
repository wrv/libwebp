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

#include "decode_webp.h"

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


int main(int argc, const char* argv[]) {
  const char* in_file = NULL;
  const char* out_file = NULL;
  const char* out_time_file = NULL;

  printf("Webp version 1.2.2\n");

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

  double dt = DecodeWebpImage(data, data_size);
  fprintf(stderr, "Time to decode pictures: %.10fs\n", dt);

  fprintf(out_time, "%f\n", dt);
  fclose(out_time);
  free((void*)data);
  return 0;
}
