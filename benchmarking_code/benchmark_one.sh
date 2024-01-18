N=20
indir=inputs_one/
infile=inputs_one/1.webp
cur_date=$(date +%s)
outputdirname=tmp/${cur_date}_just_vp8getbit_prop196

title="Just VP8GetBit - Probability 196"
runs=3

# Build the library
# change to "build" later
if [ "$1" = "build" ]; then
    echo "Building!"
    cd ..
    ./build.sh
    cd benchmarking_code
    # Build the local binaries
    make all -B
fi

for r in $(seq 1 $runs)
do
    outdir=${outputdirname}_${r}
    mkdir -p ${outdir}


    logname=${outdir}/benchmark_log.txt
    # Clear the log
    echo "" > ${logname}

    for testname in 'native' 'nativesimd' 'wasm' 'wasmsimd';
    do
        imagename=${infile##*/}
        rm ${outdir}/${imagename}_${testname}.csv > /dev/null 2>&1
        for i in $(seq 1 $N)
        do
            echo ./decode_webp_${testname} inputs/${imagename} ${outdir}/${imagename}_${testname}.csv >> ${logname}
            bin/decode_webp_${testname} inputs/${imagename} ${outdir}/${imagename}_${testname}.csv ${outdir}/${imagename}_${testname}.ppm >> ${logname} 2>&1
        done
        python3 stat_analysis.py "${outdir}/${imagename}_${testname}.csv" "${outdir}/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "${outdir}/${imagename}_${testname}_stats.png"
        objdump -d bin/decode_webp_${testname} > ${outdir}/decode_webp_${testname}.objdump
        # Copy test files
        cp bin/decode_webp_${testname}* ${outdir}/
        cp -r ../libwebp_${testname} ${outdir}/
    done

    cp decode_webp_wasm* ${outdir}/
    sha256sum ${outdir}/*.ppm

    python comp_analysis.py ${indir} ${outdir} "${title}"
done