#!/bin/bash

echo -e "Starting tests"

POLYBENCH_TESTS=(
    "linear-algebra/kernels/2mm/2mm"
    "linear-algebra/kernels/3mm/3mm"
    "linear-algebra/blas/syr2k/syr2k"
)

POLYBENCH_PATH="test/polybench"

TIMEBENCH="$POLYBENCH_PATH/utilities/time_benchmark.sh"

for i in ${!POLYBENCH_TESTS[@]};
do
    base=${POLYBENCH_TESTS[$i]}
    pluto="${base}_pluto"
    atiling="${base}_atiling"
    echo "---- Test $base ----"
    echo "---- Default"
    sh $TIMEBENCH "${POLYBENCH_PATH}/$base"
    echo "---- Pluto"
    sh $TIMEBENCH "${POLYBENCH_PATH}/$pluto"
    echo "---- Atiling"
    sh $TIMEBENCH "${POLYBENCH_PATH}/$atiling"
    
    echo -e "Done."

done