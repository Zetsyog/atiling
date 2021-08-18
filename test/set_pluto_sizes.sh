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

    parent=$(dirname $1)
    input="$parent/$PREFIX.tile.sizes"
    output="$parent/tile.sizes"

    if [[ -f $input ]]; then
        echo "replacing $input by $output"
        rm -f $output && cp $input $output
    fi

done
