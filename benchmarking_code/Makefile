CC=clang
WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
SIMDE_PATH=/home/wrv/research/wasmperf/simde-0.7.6
WABT_PATH=/home/wrv/research/wasmperf/wabt-1.0.34
UVWASI_PATH=/home/wrv/research/wasmperf/uvwasi-0.0.19

WASI_LDFLAGS=-Wl,--export=malloc,--export=DecodeWebpImage -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576
WASI_CLANG=${WASI_SDK_PATH}/bin/clang
WASI_FLAGS=-mexec-model=reactor --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot
WASI_SIMD_FLAGS=-msimd128 -I${SIMDE_PATH}

WASM2C_BIN=${WABT_PATH}/bin/wasm2c
WASM2C_DEP=${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c -I${WABT_PATH}/include
WASMSIMD_FLAGS=-DENABLE_SIMD -I${SIMDE_PATH}

# Set this to -DOUTPUT_IMAGE to produce an output file
OUTPUT_IMAGE=#-DOUTPUT_IMAGE
# Set this to -DINCREMENTAL to use the incremental decoder
INCREMENTAL=-DINCREMENTAL

COMPILE_FLAGS=-g -O2 -static ${OUTPUT_IMAGE} ${INCREMENTAL} -mavx2

ifdef OUTPUT_IMAGE
UVWASI_RESOURCES=uvwasi-rt.c $(UVWASI_PATH)/out/cmake/libuvwasi_a.a $(UVWASI_PATH)/out/cmake/_deps/libuv-build/libuv_a.a -I $(UVWASI_PATH)/include/
else
UVWASI_RESOURCES=
endif

.PHONY: native wasm wasmsimdnointrinsics

all: native wasm

clean:
	rm decode_webp_native* decode_webp_wasm*

decode_webp_native: main_native.c decode_webp.c
	${CC} ${COMPILE_FLAGS} $^ -o $@ -I../libwebp_native/include/ -L../libwebp_native/lib/ -lwebp

decode_webp_nativesimd: main_native.c decode_webp.c
	${CC} ${COMPILE_FLAGS} $^ -o $@ -I../libwebp_nativesimd/include/ -L../libwebp_nativesimd/lib/ -lwebp

decode_webp_wasm.wasm: decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} $^ -o $@ -I../libwebp_wasm/include/ -L../libwebp_wasm/lib/ -lwebp ${OUTPUT_IMAGE} ${INCREMENTAL}

decode_webp_wasm.c: decode_webp_wasm.wasm
	${WASM2C_BIN} $^ -o $@

decode_webp_wasm: main_wasm.c decode_webp_wasm.c
	${CC} ${COMPILE_FLAGS} -o $@ $^ ${WASM2C_DEP} ${UVWASI_RESOURCES}

decode_webp_wasmsimd.wasm: decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} $^ -o $@ -I../libwebp_wasmsimd/include/ -L../libwebp_wasmsimd/lib/ -lwebp ${OUTPUT_IMAGE} ${INCREMENTAL} ${WASI_SIMD_FLAGS}

decode_webp_wasmsimd.c: decode_webp_wasmsimd.wasm
	${WASM2C_BIN} $^ -o $@

decode_webp_wasmsimd: main_wasm.c decode_webp_wasmsimd.c
	${CC} ${COMPILE_FLAGS} -o $@ $^ ${WASM2C_DEP} ${UVWASI_RESOURCES} ${WASMSIMD_FLAGS}

native: decode_webp_native decode_webp_nativesimd

wasm: decode_webp_wasm decode_webp_wasmsimd

wasmsimdnointrinsics: main_wasm.c decode_webp.c
	${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_wasmsimd_no_intrin.wasm -I../libwebp_wasmsimd_no_intrinsics/include/ -L../libwebp_wasmsimd_no_intrinsics/lib/ -lwebp ${OUTPUT_IMAGE} ${INCREMENTAL} ${WASI_SIMD_FLAGS}
	${WASM2C_BIN} decode_webp_wasmsimd_no_intrin.wasm -o decode_webp_wasmsimd_no_intrin.c --module-name decode_webp_wasmsimd
	${CC} ${COMPILE_FLAGS} -o decode_webp_wasmsimd_no_intrinsics decode_webp_wasmsimd_no_intrin.c main_wasm.c ${WASM2C_DEP} ${UVWASI_RESOURCES} ${WASMSIMD_FLAGS}