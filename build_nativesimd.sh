make clean > /dev/null
echo "Building Native+SIMD version of libwebp"
curprefix=$(pwd)/libwebp_nativesimd
mkdir -p ${curprefix}

CFLAGS="-O2" \
./configure \
--prefix=${curprefix} \
--disable-libwebpdemux \
--disable-libwebpmux \
--disable-libwebpdecoder \
--disable-png \
--disable-tiff \
--disable-jpeg \
--disable-threading \
--enable-sse2 \
--enable-sse4.1

sed -i 's|#define HAVE_DLFCN_H 1|/\* #undef HAVE_DLFCN_H \*/|' src/webp/config.h


make
make install

# Backup the config file
cp src/webp/config.h ${curprefix}/config.h