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

find_tile_sizes_rec() {
    if [[ $# -ne 5 ]]; then
        echo "error"
        exit 1
    fi
    local tile_file=$1
    local source=$2
    local exec=$3
    local max_level=$4
    local level=$5

    list=$SIZE_LIST
    if [[ $level -eq $max_level ]]; then
        list+=" 1"
    fi

    for i in $list; do
        sed -i "${level}s/.*/$i/" $tile_file
        if [[ $level -eq $max_level ]]; then
            rm -f $source
            make ADDCFLAGS="-w" $exec -s >/dev/null

            score1=$($exec)
            score2=$($exec)
            score3=$($exec)
            score=$(echo "scale=5; ($score1 + $score2 + $score3) / 3" | bc)
            printf "."

            test=$(echo "$score < $best_score" | bc -l)

            if [[ $test -eq 1 ]]; then
                best_score=$score
                best_sizes=$(cat $tile_file)
            fi
        else
            find_tile_sizes_rec $tile_file $source $exec $max_level $(($level + 1))
        fi
    done
}

for bench in "${BENCHMARK_TESTS[@]}"; do
    OLDIFS=$IFS
    IFS=';'
    set -- $bench
    IFS=$OLDIFS

    exec="$1"_pluto
    source="$1_pluto_gen.c"
    tiling_level=$2
    dirname=$(dirname $exec)
    size_file="$dirname/tile.sizes"
    out="$dirname/$PREFIX.tile.sizes"

    best_score=1000
    best_sizes=()

    rm -f $size_file

    echo "bench: $exec"

    for i in $(seq 1 $tiling_level); do
        best_sizes+=(1)
        echo "1" >>$size_file
    done
    find_tile_sizes_rec $size_file $source $exec $tiling_level 1
    echo ""
    rm -f $size_file

    rm -f $out
    for i in $best_sizes; do
        echo $i >>$out
    done
    echo $best_sizes

done
