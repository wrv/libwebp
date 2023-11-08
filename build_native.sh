make clean > /dev/null
echo "Building Native version of libwebp"
curprefix=$(pwd)/libwebp_native

mkdir -p ${curprefix}

# Note: this still emits SSE instructions, though
#    it should not rely on the SSE optimized code.
#    We leave out `-mno-sse` because we get the following
#    error when we include it:
#    sharpyuv_csp.c:18:45: error: SSE register return with SSE disabled
CFLAGS="-mno-avx -mno-mmx" \
./configure \
--prefix=${curprefix} \
--disable-libwebpdemux \
--disable-libwebpmux \
--disable-libwebpdecoder \
--disable-png \
--disable-tiff \
--disable-jpeg \
--disable-threading \
--disable-sse4.1 \
--disable-sse2

sed -i 's|#define HAVE_DLFCN_H 1|/\* #undef HAVE_DLFCN_H \*/|' src/webp/config.h


make
make install

cp src/webp/config.h ${curprefix}/config.h