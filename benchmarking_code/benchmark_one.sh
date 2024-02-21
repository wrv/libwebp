N=20
indir=inputs_one/
infile=inputs_one/1.webp
cur_date=$(date +%s)
outputdirname=tmp/${cur_date}_complete_decode_alias_value_in_parse_intra_moderow_get_coeffs_vp8parseproba__alias_ptr

title="Complete Decode; Alias in VP8ParseIntraModeRow br->buffer_ br->value_ br->range_ br->bits_ store; same Alias in GetCoeffs, VP8ParseProba, a"
runs=3
decode_count=100

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

    for testname in 'native' 'nativesimd' 'wasm' 'wasmsimd';
    do
        logname=${outdir}/benchmark_log_${testname}.txt
        imagename=${infile##*/}
        rm ${outdir}/${imagename}_${testname}.csv > /dev/null 2>&1
        for i in $(seq 1 $N)
        do
            bin/decode_webp_${testname} inputs/${imagename} ${outdir}/${imagename}_${testname}.csv ${outdir}/${imagename}_${testname}.ppm ${decode_count} > ${logname} 2>&1
        done
        python3 stat_analysis.py "${outdir}/${imagename}_${testname}.csv" "${outdir}/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "${outdir}/${imagename}_${testname}_stats.png"
        objdump -d bin/decode_webp_${testname} > ${outdir}/decode_webp_${testname}.objdump
        # Copy test files
        cp bin/decode_webp_${testname}* ${outdir}/
        cp -r ../libwebp_${testname} ${outdir}/

        # Count the number of LoadNewBytes calls
        #echo "${testname} Number of VP8GetBit1 calls: "
        #grep "VP8GetBit1" ${logname} | wc -w

        #echo "${testname} Number of VP8GetBit0 calls: "
        #grep "VP8GetBit0" ${logname} | wc -w
    done

    cp decode_webp_wasm* ${outdir}/
    sha256sum ${outdir}/*.ppm
    #sha256sum ${outdir}/*.objdump

    python comp_analysis.py ${indir} ${outdir} "${title}"
done
