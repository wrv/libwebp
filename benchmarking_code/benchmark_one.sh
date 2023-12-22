N=20
logname=benchmark_log.txt
indir=inputs_one/
infile=inputs_one/1.webp
outdir=tmp/backtodecodebit_vp8getsigned_255_1k_changes_vp8getsigned
test="vp8getsigned_255_1k_times_changes_vp8getsigned"

# May not work with lossless

echo "" > ${logname}
mkdir -p ${outdir}

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
    cp bin/decode_webp_${testname}* ${outdir}/
    objdump -d bin/decode_webp_${testname} > ${outdir}/decode_webp_${testname}.objdump
done

sha256sum ${outdir}/*.ppm

python comp_analysis.py ${indir} ${outdir} "${test}"