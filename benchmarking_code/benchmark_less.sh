N=20
logname=benchmark_log.txt
output_dir=results_less

echo "" > ${logname}

mkdir -p ${output_dir}

for IMAGE in inputs_less/*.webp;
do
    for testname in 'native' 'nativesimd' 'wasm' 'wasmsimd';
    do
        imagename=${IMAGE##*/}
        rm ${output_dir}/${imagename}_${testname}.csv > /dev/null 2>&1
        for i in $(seq 1 $N)
        do
            echo ./decode_webp_${testname} inputs/${imagename} ${output_dir}/${imagename}_${testname}.csv >> ${logname}
            ./decode_webp_${testname} inputs/${imagename} ${output_dir}/${imagename}_${testname}.csv >> ${logname} 2>&1
        done
        python3 stat_analysis.py "${output_dir}/${imagename}_${testname}.csv" "${output_dir}/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "${output_dir}/${imagename}_${testname}_stats.png"
    done
done
