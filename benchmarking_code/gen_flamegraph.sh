# $1: program
# $2: input
# $3: time output
# $4: svg filename

version=$1
input=$2
output_dir=$3
#time_output=$3
#svg_filename=$4

perf record -g -- ./decode_webp_$version inputs/$input tmp/flame.txt # produces perf.data
perf script > perf.perf
../../FlameGraph/stackcollapse-perf.pl perf.perf > perf.folded
../../FlameGraph/flamegraph.pl perf.folded > $output_dir/$input.$version.flame.svg