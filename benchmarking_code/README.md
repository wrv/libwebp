# Benchmarking libwebp

This code is used to measure the decoding time of libwebp between Native/WASM and no-SIMD/SIMD.

**This codebase has changes that enable the use of SIMD-everywhere for WASMSIMD**.


## Building


### Dependencies

Building the native version of libwebp is pretty straight forward - you just need clang.

Compiling the WASM version of libwebp requires some dependencies.
- wasi-sdk: https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz
- wabt: https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
- simde: https://github.com/simd-everywhere/simde/archive/refs/tags/v0.7.6.tar.gz

wasi-sdk, wabt, and simde should work out-of-the-box. 

The analysis script `benchmarking_code/stat_analysis.py` requires numpy and matplotlib. These can be installed with  `pip install numpy matplotlib`.

### Building libwebp variants

In `build.sh` found in the top-most directory, change the following environment variables:
```bash
WASI_SDK_PATH=/path/to/wasi-sdk-20.0
SIMDE_PATH=/path/to/simde-0.7.6
WABT_PATH=/path/to/wabt-1.0.34
```

With those set, run `./build.sh`. This directory will contain the following binaries used for benchmarking:
- `decode_webp_native`: Native non-SIMD
- `decode_webp_nativesimd`: Native SIMD
- `decode_webp_wasm`: WASM non-SIMD
- `decode_webp_wasmsimd`: WASM SIMD

## Running benchmarks




## Results

On input `./test.webp`:

|             | **Native** | **WASM** |
|:-----------:|:----------:|:--------:|
| **No SIMD** |            |          |
| **SIMD**    |            |          |


## Performance Issues

A big performance difference is coming from `VP8ParseIntraModeRow` between Native and WASM.