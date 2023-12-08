##
# Builds 4 different versions of libwebp for benchmarking
# Requires
# - wasi-sdk: https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz
# - wabt: https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
# - simde: https://github.com/simd-everywhere/simde/archive/refs/tags/v0.7.6.tar.gz

WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
SIMDE_PATH=/home/wrv/research/wasmperf/simde-0.7.6
curdir=$(pwd)

./autogen.sh

#################################################

./build_native.sh

#################################################
./build_nativesimd.sh

#################################################
./build_wasm.sh

#################################################
./build_wasmsimd.sh
./build_wasmsimd_no_intrinsics.sh

#./build_wasmsimd_sse2.sh
#./build_wasmsimd_sse41.sh
#./build_wasmsimd_neon.sh