CC = clang
WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
SIMDE_PATH=/home/wrv/research/wasmperf/simde-0.7.6
WABT_PATH=/home/wrv/research/wasmperf/wabt-1.0.34
UVWASI_PATH=/home/wrv/research/wasmperf/uvwasi-0.0.19

## Set this -DOUTPUT_IMAGE to produce an output file
OUTPUT_IMAGE=-DOUTPUT_IMAGE

WASI_LDFLAGS=-Wl,--export=malloc,--export=DecodeWebpImage -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576
WASI_CLANG=${WASI_SDK_PATH}/bin/clang
WASI_FLAGS=-mexec-model=reactor --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot
WASM2C_BIN=${WABT_PATH}/bin/wasm2c
UVWASI_RESOURCES=uvwasi-rt.c $(UVWASI_PATH)/out/cmake/libuvwasi_a.a $(UVWASI_PATH)/out/cmake/_deps/libuv-build/libuv_a.a -I $(UVWASI_PATH)/include/

WASM2C_DEP=${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c
WASM2C_INCLUDE=-I${WABT_PATH}/include -I${SIMDE_PATH}
WASM_SIMD_FLAGS=-msimd128 -I${SIMDE_PATH}
BUILD_INCLUDE=-I../libwebp_$@/include/ -L../libwebp_$@/lib/ -lwebp -I${SIMDE_PATH}
COMPILE_FLAGS=-g -O2 -static ${OUTPUT_IMAGE}

COMPILE_WASI_CLANG=${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm ${BUILD_INCLUDE} ${OUTPUT_IMAGE}
RUN_WASM2C=${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c

ifdef OUTPUT_IMAGE
COMPILE_WASM2C_OUTPUT=clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP} ${UVWASI_RESOURCES}
else
COMPILE_WASM2C_OUTPUT=clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP}
endif

.PHONY: native nativesimd wasm wasmsimd

all: native nativesimd wasm wasmsimd

clean:
	rm decode_webp_native* decode_webp_wasm*

native: main_native.c decode_webp.c
	${CC} ${COMPILE_FLAGS} $^ -o decode_webp_$@ ${BUILD_INCLUDE}

nativesimd: main_native.c decode_webp.c
	${CC} ${COMPILE_FLAGS} $^ -o decode_webp_$@ ${BUILD_INCLUDE}

wasm: main_wasm.c decode_webp.c
	${COMPILE_WASI_CLANG}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}

wasmsimd: main_wasmsimd.c decode_webp.c
	${COMPILE_WASI_CLANG} ${WASM_SIMD_FLAGS}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}
