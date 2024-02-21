prev_dir=$1
new_dir=$2
testname=$3 # native, nativesimd, wasm, wasmsimd

infile=inputs/1.webp
imagename=${infile##*/}
outputdirname=tmp/$(date +%s)_comparison
mkdir -p ${outputdirname}

echo "Working directory: ${outputdirname}"

logname=${outputdirname}/benchmark_log.txt
echo "" > ${logname}

runs=3
N=20

############################
## Copy relevant files
############################

cp ${prev_dir}/decode_webp_${testname} ${outputdirname}/decode_webp_${testname}_prev
cp ${new_dir}/decode_webp_${testname} ${outputdirname}/decode_webp_${testname}_new

cp ${prev_dir}/decode_webp_${testname}.objdump ${outputdirname}/decode_webp_${testname}_prev.objdump
cp ${new_dir}/decode_webp_${testname}.objdump ${outputdirname}/decode_webp_${testname}_new.objdump

if [ $testname = "wasmsimd" ]; then
    cp ${prev_dir}/decode_webp_wasmsimd.c ${outputdirname}/decode_webp_wasmsimd_prev.c
    cp ${new_dir}/decode_webp_wasmsimd.c ${outputdirname}/decode_webp_wasmsimd_new.c
fi

############################
## Run the previous version
############################
for i in $(seq 1 $N)
do
    ${outputdirname}/decode_webp_${testname}_prev ${infile} ${outputdirname}/${imagename}_${testname}_prev.csv ${outputdirname}/${imagename}_${testname}_prev.ppm >> ${logname} 2>&1
done

############################
## Run the newer version
############################

for i in $(seq 1 $N)
do
    ${outputdirname}/decode_webp_${testname}_new ${infile} ${outputdirname}/${imagename}_${testname}_new.csv ${outputdirname}/${imagename}_${testname}_new.ppm >> ${logname} 2>&1
done

############################
## Run the newer version again
############################

for i in $(seq 1 $N)
do
    ${outputdirname}/decode_webp_${testname}_new ${infile} ${outputdirname}/${imagename}_${testname}_new.csv ${outputdirname}/${imagename}_${testname}_new.ppm >> ${logname} 2>&1
done

############################
## Run the previous version again
############################
for i in $(seq 1 $N)
do
    ${outputdirname}/decode_webp_${testname}_prev ${infile} ${outputdirname}/${imagename}_${testname}_prev.csv ${outputdirname}/${imagename}_${testname}_prev.ppm >> ${logname} 2>&1
done


############################
## Compare results
############################

sha256sum ${outputdirname}/*.ppm

python3 stat_analysis.py "${outputdirname}/${imagename}_${testname}_prev.csv" "${outputdirname}/${imagename}_${testname}_stats_prev.txt" "PREV: ${imagename} with ${testname}" "${outputdirname}/${imagename}_${testname}_prev_stats.png"
python3 stat_analysis.py "${outputdirname}/${imagename}_${testname}_new.csv" "${outputdirname}/${imagename}_${testname}_stats_new.txt" "NEW: ${imagename} with ${testname}" "${outputdirname}/${imagename}_${testname}_new_stats.png"

diff ${outputdirname}/decode_webp_${testname}_prev.objdump ${outputdirname}/decode_webp_${testname}_new.objdump > ${outputdirname}/decode_webp_${testname}_prev_vs_new.objdump.diff

if [ $testname = "wasmsimd" ]; then
    diff ${outputdirname}/decode_webp_wasmsimd_prev.c ${outputdirname}/decode_webp_wasmsimd_new.c > ${outputdirname}/decode_webp_wasmsimd_prev_vs_new.c.diff
fi

#python comp_analysis.py ${indir} ${outputdirname} "${title}"
