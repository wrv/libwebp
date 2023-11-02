make clean > /dev/null
echo "Building Native+SIMD version of libwebp"
curprefix=$(pwd)/libwebp_nativesimd
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