WASI_SDK_PATH=/home/wrv/research/wasmperf/wasi-sdk-20.0
SIMDE_PATH=/home/wrv/research/wasmperf/simde-0.7.6
WABT_PATH=/home/wrv/research/wasmperf/wabt-1.0.34

WASI_LDFLAGS=-Wl,--export=malloc -Wl,--no-entry -Wl,--growable-table -Wl,--stack-first -Wl,-z,stack-size=1048576
WASI_CLANG=${WASI_SDK_PATH}/bin/clang
WASI_FLAGS=-mexec-model=reactor --sysroot ${WASI_SDK_PATH}/share/wasi-sysroot
WASM2C_BIN=${WABT_PATH}/bin/wasm2c
WASM2C_DEP=${WABT_PATH}/share/wabt/wasm2c/wasm-rt-impl.c ${WABT_PATH}/share/wabt/wasm2c/wasm-rt-exceptions-impl.c
WASM2C_INCLUDE=-I${WABT_PATH}/include -I${SIMDE_PATH}
WASM_SIMD_FLAGS=-msimd128 -I${SIMDE_PATH}
BUILD_INCLUDE=-I../libwebp_$@/include/ -L../libwebp_$@/lib/ -lwebp -I${SIMDE_PATH}
COMPILE_FLAGS=-O3 -mavx -g -fxray-instrument -static

COMPILE_NATIVE=clang ${COMPILE_FLAGS} decode_webp.c main_native.c -o decode_webp_$@ ${BUILD_INCLUDE}
COMPILE_WASI_CLANG=${WASI_CLANG} ${WASI_LDFLAGS} ${WASI_FLAGS} decode_webp.c -o decode_webp_$@.wasm ${BUILD_INCLUDE}
RUN_WASM2C=${WASM2C_BIN} decode_webp_$@.wasm -o decode_webp_$@.c
COMPILE_WASM2C_OUTPUT=clang ${COMPILE_FLAGS} -o decode_webp_$@ ${WASM2C_INCLUDE} main_$@.c decode_webp_$@.c ${WASM2C_DEP}

.PHONY: native nativesimd wasm wasmsimd benchmark

all: native nativesimd wasm wasmsimd

clean:
	rm decode_webp_native decode_webp_nativesimd decode_webp_wasm*

native: main_native.c decode_webp.c
	${COMPILE_NATIVE}

nativesimd: main_native.c decode_webp.c
	${COMPILE_NATIVE}

wasm: main_wasm.c decode_webp.c
	${COMPILE_WASI_CLANG}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}

wasmsimd: main_wasmsimd.c decode_webp.c
	${COMPILE_WASI_CLANG} ${WASM_SIMD_FLAGS}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}

wasmsimd_sse2: main_wasmsimd_sse2.c decode_webp.c
	${COMPILE_WASI_CLANG} ${WASM_SIMD_FLAGS}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}

wasmsimd_neon: main_wasmsimd_neon.c decode_webp.c
	${COMPILE_WASI_CLANG} ${WASM_SIMD_FLAGS}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}

wasmsimd_sse41: main_wasmsimd_sse41.c decode_webp.c
	${COMPILE_WASI_CLANG} ${WASM_SIMD_FLAGS}
	${RUN_WASM2C}
	${COMPILE_WASM2C_OUTPUT}