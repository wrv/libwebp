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
#include <sys/time.h>

#include "../imageio/image_enc.h"
#include "../imageio/webpdec.h"

typedef struct timeval Stopwatch;

static WEBP_INLINE void StopwatchReset(Stopwatch* watch) {
  gettimeofday(watch, NULL);
}

static WEBP_INLINE double StopwatchReadAndReset(Stopwatch* watch) {
  struct timeval old_value;
  double delta_sec, delta_usec;
  memcpy(&old_value, watch, sizeof(old_value));
  gettimeofday(watch, NULL);
  delta_sec = (double)watch->tv_sec - old_value.tv_sec;
  delta_usec = (double)watch->tv_usec - old_value.tv_usec;
  return delta_sec + delta_usec / 1000000.0;
}


static int SaveOutput(const WebPDecBuffer* const buffer,
                      WebPOutputFileFormat format, const char* const out_file) {
  const int use_stdout = (out_file != NULL) && !strcmp(out_file, "-");
  int ok = 1;
  Stopwatch stop_watch;

  StopwatchReset(&stop_watch);
  
  ok = WebPSaveImage(buffer, format, out_file);

  if (ok) {

    if (use_stdout) {
      fprintf(stderr, "Saved to stdout\n");
    } else {
      fprintf(stderr, "Saved file %s\n", (const char*)out_file);
    }
    const double write_time = StopwatchReadAndReset(&stop_watch);
    fprintf(stderr, "Time to write output: %.3fs\n", write_time);
  
  } else {
    if (use_stdout) {
      fprintf(stderr, "Error writing to stdout !!\n");
    } else {
      fprintf(stderr, "Error writing file %s !!\n", (const char*)out_file);
    }
  }
  return ok;
}

static void Help(void) {
  printf("Usage: dwebp in_file [options] [-o out_file]\n\n"
         "Decodes the WebP image file to YUV format [Default].\n"
         "Note: Animated WebP files are not supported.\n\n"
        );
}

static const char* const kFormatType[] = {
  "unspecified", "lossy", "lossless"
};


int main(int argc, const char* argv[]) {
  int ok = 0;
  const char* in_file = NULL;
  const char* out_file = NULL;

  WebPDecoderConfig config;
  WebPDecBuffer* const output_buffer = &config.output;
  WebPBitstreamFeatures* const bitstream = &config.input;
  WebPOutputFileFormat format = RAW_YUV;
  const uint8_t* data = NULL;

  int c;
  printf("Webp version 1.2.2\n");

  if (!WebPInitDecoderConfig(&config)) {
    fprintf(stderr, "Library version mismatch!\n");
    return -1;
  }

  for (c = 1; c < argc; ++c) {
    if (!strcmp(argv[c], "-h") || !strcmp(argv[c], "-help")) {
      Help();
      return 0;
    } else if (!strcmp(argv[c], "-o") && c < argc - 1) {
      out_file = (const char*) argv[++c];
    } else if (!strcmp(argv[c], "--")) {
      if (c < argc - 1) in_file = (const char*)argv[++c];
      break;
    } else if (argv[c][0] == '-') {
      fprintf(stderr, "Unknown option '%s'\n", argv[c]);
      Help();
      return -1;
    } else {
      in_file = (const char*)argv[c];
    }
  }

  if (in_file == NULL) {
    fprintf(stderr, "missing input file!!\n");
    Help();
    return -1;
  }

  VP8StatusCode status = VP8_STATUS_OK;
  size_t data_size = 0;
  if (!LoadWebP(in_file, &data, &data_size, bitstream)) {
    return -1;
  }

  // Appropriate colorspace for YUV output stream
  output_buffer->colorspace = bitstream->has_alpha ? MODE_YUVA : MODE_YUV;

  Stopwatch stop_watch;
  StopwatchReset(&stop_watch);
  status = DecodeWebP(data, data_size, &config);
  const double decode_time = StopwatchReadAndReset(&stop_watch);
  fprintf(stderr, "Time to decode picture: %.10fs\n", decode_time);
  
  ok = (status == VP8_STATUS_OK);
  if (!ok) {
    PrintWebPError(in_file, status);
    goto Exit;
  }
  
  if (out_file != NULL) {
    fprintf(stderr, "Decoded %s.", (const char*)in_file);
    fprintf(stderr, " Dimensions: %d x %d %s. Format: %s. Now saving...\n",
            output_buffer->width, output_buffer->height,
            bitstream->has_alpha ? " (with alpha)" : "",
            kFormatType[bitstream->format]);
    
    ok = SaveOutput(output_buffer, format, out_file);
  } else {
    fprintf(stderr, "File %s can be decoded ", (const char*)in_file);
    fprintf(stderr, "(dimensions: %d x %d %s. Format: %s).\n",
            output_buffer->width, output_buffer->height,
            bitstream->has_alpha ? " (with alpha)" : "",
            kFormatType[bitstream->format]);
    fprintf(stderr, "Nothing written; "
                    "use -o flag to save the result as e.g. PNG.\n");
  
  }
 Exit:
  WebPFreeDecBuffer(output_buffer);
  WebPFree((void*)data);
  return ok ? 0 : -1;
}

//------------------------------------------------------------------------------
