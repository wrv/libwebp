##
# Builds 4 different versions of libwebp for benchmarking
# Requires
# - wasi-sdk: https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz
# - wabt: https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
# - simde: https://github.com/simd-everywhere/simde/archive/refs/tags/v0.7.6.tar.gz
# - uvwasi: https://github.com/nodejs/uvwasi/archive/refs/tags/v0.0.19.tar.gz

WASI_SDK_PATH=/home/dev/research/wasi-sdk-20.0
SIMDE_PATH=/home/dev/research/simde-0.7.6
curdir=$(pwd)

./autogen.sh

#################################################

make clean > /dev/null
echo "Building Native version of libwebp"
curprefix=${curdir}/libwebp_native

mkdir -p ${curprefix}

# Note: this still emits SSE instructions, though
#    it should not rely on the SSE optimized code.
#    We leave out `-mno-sse` because we get the following
#    error when we include it:
#    sharpyuv_csp.c:18:45: error: SSE register return with SSE disabled
CFLAGS="-mno-avx -mno-mmx" \
./configure \
--prefix=${curprefix} \
--enable-libwebpdemux \
--enable-libwebpmux \
--enable-libwebpdecoder \
--disable-sse4.1 \
--disable-sse2
make
make install

#################################################
make clean > /dev/null
echo "Building Native+SIMD version of libwebp"
curprefix=${curdir}/libwebp_nativesimd
mkdir -p ${curprefix}

./configure \
--prefix=${curprefix} \
--enable-libwebpdemux \
--enable-libwebpmux \
--enable-libwebpdecoder \
--enable-sse4.1 \
--enable-sse2
make
make install

#################################################
make clean > /dev/null
echo "Building WASM version of libwebp"
curprefix=${curdir}/libwebp_wasm
mkdir -p ${curprefix}

CFLAGS="-D_WASI_EMULATED_SIGNAL" \
LDFLAGS="-L${WASI_SDK_PATH}/share/wasi-sysroot/lib \
	-Wl,--no-entry \
	-Wl,--export-all \
	-Wl,--growable-table $*" \
LD=${WASI_SDK_PATH}/bin/wasm-ld \
CC=${WASI_SDK_PATH}/bin/clang \
AR=${WASI_SDK_PATH}/bin/ar \
LIBS=-lwasi-emulated-signal \
STRIP=${WASI_SDK_PATH}/bin/strip \
RANLIB=${WASI_SDK_PATH}/bin/ranlib \
./configure \
--with-sysroot=${WASI_SDK_PATH}/share/wasi-sysroot \
--host=wasm32 \
--prefix=${curprefix} \
--enable-libwebpdemux \
--enable-libwebpmux \
--enable-libwebpdecoder
make
make install

#################################################
make clean > /dev/null
echo "Building WASM+SIMD version of libwebp"
curprefix=${curdir}/libwebp_wasmsimd
mkdir -p ${curprefix}

CFLAGS="-msimd128 -DWEBP_USE_SIMDE -DSIMDE_ENABLE_NATIVE_ALIASES -I${SIMDE_PATH} -D_WASI_EMULATED_SIGNAL" \
LDFLAGS="-L${WASI_SDK_PATH}/share/wasi-sysroot/lib \
	-Wl,--no-entry \
	-Wl,--export-all \
	-Wl,--growable-table $*" \
LIBS=-lwasi-emulated-signal \
LD=${WASI_SDK_PATH}/bin/wasm-ld \
CC=${WASI_SDK_PATH}/bin/clang \
AR=${WASI_SDK_PATH}/bin/ar \
STRIP=${WASI_SDK_PATH}/bin/strip \
RANLIB=${WASI_SDK_PATH}/bin/ranlib \
./configure \
--with-sysroot=${WASI_SDK_PATH}/share/wasi-sysroot \
--host=wasm32 \
--prefix=${curprefix} \
--enable-libwebpdemux \
--enable-libwebpmux \
--enable-libwebpdecoder \
--enable-sse4.1 \
--enable-sse2

make
make install