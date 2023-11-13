if [ "$WASI_SDK_PATH" == "" ]; then
    WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
fi

make clean > /dev/null
echo "Building WASM+SIMD version of libwebp (autovectorized; no intrinsics)"
curprefix=$(pwd)/libwebp_wasmsimd
mkdir -p ${curprefix}

CFLAGS="-msimd128 -O2 -D_WASI_EMULATED_SIGNAL" \
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
--disable-libwebpdemux \
--disable-libwebpmux \
--disable-libwebpdecoder \
--disable-png \
--disable-tiff \
--disable-jpeg \
--disable-threading \
--disable-sse2

make
make install

# Backup the config file
cp src/webp/config.h ${curprefix}/config.h