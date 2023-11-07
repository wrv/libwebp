WASI_SDK_PATH=/home/dev/research/wasi-sdk-20.0
SIMDE_PATH=/home/dev/research/simde-0.7.6
WABT_PATH=/home/dev/research/wabt-1.0.34
UVWASI_PATH=/home/dev/research/uvwasi-0.0.19

WASI_LDFLAGS=-Wl,--export=malloc -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576
WASI_CLANG=${WASI_SDK_PATH}/bin/clang
WASI_FLAGS=-mexec-model=reactor --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot
WASM2C_BIN=${WABT_PATH}/bin/wasm2c
WASM2C_DEP=uvwasi-rt.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c ${UVWASI_PATH}/out/cmake/libuvwasi_a.a ${UVWASI_PATH}/out/cmake/_deps/libuv-build/libuv_a.a
WASM2C_INCLUDE=-I${WABT_PATH}/include -I${UVWASI_PATH}/include
#WASM2C_DEP=${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c
#WASM2C_INCLUDE=-I${WABT_PATH}/include
NATIVE_SIMD_FLAGS=-I${SIMDE_PATH} -mavx
WASM_SIMD_FLAGS=-msimd128 -I${SIMDE_PATH}
BUILD_INCLUDE=-I../libwebp_$@/include/ -L../libwebp_$@/lib/ -lwebp
COMPILE_FLAGS=-O3 -g -fxray-instrument

.PHONY: native nativesimd wasm wasmsimd benchmark

all: native nativesimd wasm wasmsimd

clean:
	rm decode_webp_native decode_webp_nativesimd decode_webp_wasm decode_webp_wasmsimd decode_webp_wasm.* decode_webp_wasmsimd.*

native: main_native.c decode_webp.c
	clang ${COMPILE_FLAGS} decode_webp.c main_native.c -o decode_webp_$@ ${BUILD_INCLUDE}

nativesimd: main_native.c decode_webp.c
	clang ${COMPILE_FLAGS} main_native.c decode_webp.c -o decode_webp_$@ ${BUILD_INCLUDE} ${NATIVE_SIMD_FLAGS}

wasm: main_wasm.c decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm ${BUILD_INCLUDE}
	${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c
	clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP}

wasmsimd: main_wasmsimd.c decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm ${BUILD_INCLUDE} ${WASM_SIMD_FLAGS}
	${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c
	clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP} ${NATIVE_SIMD_FLAGS}

wasmsimd_sse2: main_wasmsimd.c decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm ${BUILD_INCLUDE} ${WASM_SIMD_FLAGS}
	${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c
	clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP} ${NATIVE_SIMD_FLAGS}

wasmsimd_sse41: main_wasmsimd.c decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm -I../libwebp_$@/include/ -L../libwebp_$@/lib/ -lwebp ${WASM_SIMD_FLAGS}
	${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c
	clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP} ${NATIVE_SIMD_FLAGS}