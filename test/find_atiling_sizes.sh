#!/usr/bin/env bash

if [[ $# -ne 1 ]]; then
    echo "usage: $0 [prefix]"
    exit 1
fi
PREFIX=$1

. ./bench_list.sh

SIZE_LIST="8 16"

best_score=""
best_sizes=""
current_sizes=()

find_tile_sizes_rec() {
    if [[ $# -ne 4 ]]; then
        echo "error"
        exit 1
    fi
    local source=$1
    local exec=$2
    local max_level=$3
    local level=$4

    for i in $SIZE_LIST; do
        current_sizes[$level]=$i

        if [[ $level -eq $max_level ]]; then
            rm $exec
            FLAGS="-w "
            for i in $(seq 1 $max_level); do
                FLAGS+="-DATILING_DIV$i=${current_sizes[$i]} "
            done
            make ADDCFLAGS="$FLAGS" $exec >/dev/null

            score1=$($exec)
            score2=$($exec)
            score3=$($exec)
            score=$(echo "scale=5; ($score1 + $score2 + $score3) / 3" | bc)

            printf "."

            test=$(echo "$score < $best_score" | bc -l)

            if [[ $test -eq 1 ]]; then
                best_score=$score
                best_sizes=(${current_sizes[*]})
            fi
        else
            find_tile_sizes_rec $source $exec $max_level $(($level + 1))
        fi
    done
}

for bench in "${BENCHMARK_TESTS[@]}"; do
    OLDIFS=$IFS
    IFS=';'
    set -- $bench
    IFS=$OLDIFS

    exec="$1"_atiling
    source="$1_atiling_gen.c"
    tiling_level=$2
    dirname=$(dirname $exec)
    out="$dirname/$PREFIX.atiling.sizes"

    current_sizes=()
    best_score=1000
    best_sizes=()

    echo "bench: $exec"
    for i in $(seq 0 $tiling_level); do
        best_sizes+=(1)
        current_sizes+=(1)
    done

    find_tile_sizes_rec $source $exec $tiling_level 1
    echo ""

    rm -f $out
    for i in $(seq 1 $tiling_level); do
        echo "${best_sizes[$i]}" >>$out
    done
    echo ${best_sizes[*]}

done
