#!/usr/bin/env bash

if [[ $# -ne 1 ]]; then
    echo "usage: $0 [prefix_name]"
    exit 1
fi

PREFIX=$1

. ./bench_list.sh

for bench in "${BENCHMARK_TESTS[@]}"; do
    OLDIFS=$IFS
    IFS=';'
    set -- $bench
    IFS=$OLDIFS

    target=$1
    parent=$(dirname $1)

    pluto_target="${target}_pluto"
    pluto_obj="${target}_pluto_gen.o"
    pluto_input="$parent/$PREFIX.tile.sizes"
    pluto_output="$parent/tile.sizes"

    atiling_input="$parent/$PREFIX.atiling.sizes"
    atiling_target="${target}_atiling"
    atiling_obj="${target}_atiling_gen.o"

    if [[ -f $pluto_input ]]; then
        echo "replacing $pluto_input by $pluto_output"
        rm -f $output && cp $pluto_input $pluto_output
        rm -f $pluto_target $pluto_obj
        make -s $pluto_target
    if [[ -f $atiling_input ]]; then
        rm -f $atiling_target $atiling_obj
        asizes=$(cat $atiling_input)
        FLAGS=""
        i=1
        for size in $asizes; do
            FLAGS+="-DATILING_DIV$i=${size} "
            i=$((i + 1))
        done
        make ADDCFLAGS="$FLAGS" -s $atiling_target 
    fi

done
