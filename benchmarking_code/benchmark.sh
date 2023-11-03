N=20
logname=benchmark_log.txt
SAVE_OUTPUT=1

echo "" > ${logname}



for IMAGE in inputs/*.webp;
do
    for testname in 'native' 'nativesimd' 'wasm' 'wasmsimd';
    do
        imagename=${IMAGE##*/}
        rm results/${imagename}_${testname}.csv > /dev/null 2>&1
        for i in $(seq 1 $N)
        do
            echo ./decode_webp_${testname} inputs/${imagename} outputs/${imagename}.yuv results/${imagename}_${testname}.csv ${SAVE_OUTPUT} >> ${logname}
            ./decode_webp_${testname} inputs/${imagename} outputs/${imagename}_${testname}_${i}.yuv results/${imagename}_${testname}.csv ${SAVE_OUTPUT} >> ${logname} 2>&1
        done
        python3 stat_analysis.py "results/${imagename}_${testname}.csv" "results/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "results/${imagename}_${testname}_stats.png"
    done
done
