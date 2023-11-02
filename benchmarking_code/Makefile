WASI_SDK_PATH=/home/dev/research/wasi-sdk-20.0
SIMDE_PATH=/home/dev/research/simde-0.7.6
WABT_PATH=/home/dev/research/wabt-1.0.34
UVWASI_PATH=/home/dev/research/uvwasi-0.0.19

.PHONY: native nativesimd wasm wasmsimd benchmark

all: native nativesimd wasm wasmsimd

clean:
	rm simple_dwebp_native simple_dwebp_nativesimd simple_dwebp_wasm simple_dwebp_wasmsimd simple_dwebp_wasm.* simple_dwebp_wasmsimd.*

native: simple_dwebp.c
	clang simple_dwebp.c ../imageio/webpdec.c ../imageio/image_enc.c ../imageio/imageio_util.c ../imageio/metadata.c -o simple_dwebp_native -I../libwebp_native/include/ -L../libwebp_native/lib/ -lwebp -lwebpdemux

nativesimd: simple_dwebp.c
	clang simple_dwebp.c ../imageio/webpdec.c ../imageio/image_enc.c ../imageio/imageio_util.c ../imageio/metadata.c -o simple_dwebp_nativesimd -I../libwebp_nativesimd/include/ -L../libwebp_nativesimd/lib/ -lwebp -lwebpdemux -mavx

wasm: wasm2c_main.c simple_dwebp.c
	LDFLAGS="-Wl,--export-all -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576" ${WASI_SDK_PATH}/bin/clang --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot simple_dwebp.c ../imageio/webpdec.c ../imageio/image_enc.c ../imageio/imageio_util.c ../imageio/metadata.c -o simple_dwebp_wasm.wasm -I../libwebp_wasm/include/ -L../libwebp_wasm/lib/ -lwebp -lwebpdemux
	${WABT_PATH}/bin/wasm2c simple_dwebp_wasm.wasm  -o simple_dwebp_wasm.c
	clang -O3 -o simple_dwebp_wasm -I${WABT_PATH}/include -I${UVWASI_PATH}/include simple_dwebp_wasm.c uvwasi-rt.c wasm2c_main.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c ${UVWASI_PATH}/out/cmake/libuvwasi_a.a ${UVWASI_PATH}/out/cmake/_deps/libuv-build/libuv_a.a

wasmsimd: wasm2c_mainsimd.c simple_dwebp.c
	LDFLAGS="-Wl,--export-all -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576" ${WASI_SDK_PATH}/bin/clang -msimd128 --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot simple_dwebp.c ../imageio/webpdec.c ../imageio/image_enc.c ../imageio/imageio_util.c ../imageio/metadata.c -o simple_dwebp_wasmsimd.wasm -I../libwebp_wasmsimd/include/ -I${SIMDE_PATH} -L../libwebp_wasmsimd/lib/ -lwebp -lwebpdemux
	${WABT_PATH}/bin/wasm2c simple_dwebp_wasmsimd.wasm  -o simple_dwebp_wasmsimd.c
	clang -O3 -o simple_dwebp_wasmsimd -I${WABT_PATH}/include -I${UVWASI_PATH}/include -I${SIMDE_PATH} simple_dwebp_wasmsimd.c uvwasi-rt.c wasm2c_mainsimd.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c ${UVWASI_PATH}/out/cmake/libuvwasi_a.a ${UVWASI_PATH}/out/cmake/_deps/libuv-build/libuv_a.a -mavx