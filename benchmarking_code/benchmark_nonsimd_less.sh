N=5
logname=benchmark_log.txt
output_dir=results_nonsimd

echo "" > ${logname}

mkdir -p ${output_dir}


for testname in 'native' 'wasm';
do
    imagename=1.webp
    rm ${output_dir}/${imagename}_${testname}.csv > /dev/null 2>&1
    for i in $(seq 1 $N)
    do
        echo ./decode_webp_${testname} inputs/${imagename} ${output_dir}/${imagename}_${testname}.csv >> ${logname}
        ./decode_webp_${testname} inputs/${imagename} ${output_dir}/${imagename}_${testname}.csv >> ${logname} 2>&1
    done
    python3 stat_analysis.py "${output_dir}/${imagename}_${testname}.csv" "${output_dir}/${imagename}_${testname}_stats.txt" "${imagename} with ${testname}" "${output_dir}/${imagename}_${testname}_stats.png"
done
