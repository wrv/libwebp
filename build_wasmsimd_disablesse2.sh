if [ "$WASI_SDK_PATH" == "" ]; then
    WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
fi

if [ "$SIMDE_PATH" == "" ]; then
    SIMDE_PATH=/home/wrv/research/wasmperf/simde-0.7.6
fi

make clean > /dev/null
echo "Building WASM+SIMD version of libwebp"
curprefix=$(pwd)/libwebp_wasmsimd
mkdir -p ${curprefix}

CFLAGS="-O2 -msimd128 -DWEBP_WASM -DWEBP_USE_SIMDE -DSIMDE_ENABLE_NATIVE_ALIASES -I${SIMDE_PATH} -D_WASI_EMULATED_SIGNAL" \
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
--disable-libwebpdemux \
--disable-libwebpmux \
--disable-libwebpdecoder \
--disable-png \
--disable-tiff \
--disable-jpeg \
--disable-threading \
--disable-sse2 \
--disable-sse4.1


make
make install

# Backup the config file
cp src/webp/config.h ${curprefix}/config.h