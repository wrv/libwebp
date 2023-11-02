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
--enable-libwebpdemux \
--enable-libwebpmux \
--enable-libwebpdecoder \
--disable-sse4.1 \
--disable-sse2
make
make install