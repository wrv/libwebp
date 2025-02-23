N=20
logname=benchmark_log.txt
indir=inputs
outdir=tmp/results_w_output

echo "" > ${logname}

mkdir -p ${outdir}

for IMAGE in ${indir}/*.webp;
do
    for testname in 'native' 'nativesimd' 'wasm' 'wasmsimd';
    do
        imagename=${IMAGE##*/}
        rm ${outdir}/${imagename}_${testname}.csv > /dev/null 2>&1
        for i in $(seq 1 $N)
        do
            echo bin/decode_webp_${testname} inputs/${imagename} ${outdir}/${imagename}_${testname}.csv >> ${logname}
            bin/decode_webp_${testname} inputs/${imagename} ${outdir}/${imagename}_${testname}.csv ${outdir}/${imagename}_${testname}.ppm >> ${logname} 2>&1
        done
        python3 stat_analysis.py "${outdir}/${imagename}_${testname}.csv" "${outdir}/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "${outdir}/${imagename}_${testname}_stats.png"
    done
done
