#!/usr/bin/env bash

echo -e "Starting tests"

. ./bench_list.sh

## time_benchmark.sh for  in /Users/pouchet
##
## Made by Louis-Noel Pouchet
## Contact: <pouchet@cse.ohio-state.edu>
##
## Started on  Sat Oct 29 00:03:48 2011 Louis-Noel Pouchet
## Last update Fri Apr 22 15:39:13 2016 Louis-Noel Pouchet
##

## Maximal variance accepted between the 3 median runs for performance results.
## Here 5%
VARIANCE_ACCEPTED=5

compute_mean_exec_time() {
    file="$1"
    benchcomputed="$2"
    cat "$file" | grep "[0-9]\+" | sort -n | head -n 4 | tail -n 3 >avg.out
    expr="(0"
    while read n; do
        expr="$expr+$n"
    done <avg.out
    time=$(echo "scale=8;$expr)/3" | bc)
    tmp=$(echo "$time" | cut -d '.' -f 1)
    if [ -z "$tmp" ]; then
        time="0$time"
    fi
    val1=$(cat avg.out | head -n 1)
    val2=$(cat avg.out | head -n 2 | tail -n 1)
    val3=$(cat avg.out | head -n 3 | tail -n 1)
    val11=$(echo "a=$val1 - $time;if(0>a)a*=-1;a" | bc 2>&1)
    test_err=$(echo "$val11" | grep error)
    if ! [ -z "$test_err" ]; then
        echo "[ERROR] Program output does not match expected single-line with time."
        echo "[ERROR] The program must be a PolyBench, compiled with -DPOLYBENCH_TIME"
        exit 1
    fi
    val12=$(echo "a=$val2 - $time;if(0>a)a*=-1;a" | bc)
    val13=$(echo "a=$val3 - $time;if(0>a)a*=-1;a" | bc)
    myvar=$(echo "$val11 $val12 $val13" | awk '{ if ($1 > $2) { if ($1 > $3) print $1; else print $3; } else { if ($2 > $3) print $2; else print $3; } }')
    variance=$(echo "scale=5;($myvar/$time)*100" | bc)
    tmp=$(echo "$variance" | cut -d '.' -f 1)
    if [ -z "$tmp" ]; then
        variance="0$variance"
    fi
    compvar=$(echo "$variance $VARIANCE_ACCEPTED" | awk '{ if ($1 < $2) print "ok"; else print "error"; }')
    if [ "$compvar" = "error" ]; then
        echo "[WARNING] Variance is above thresold, unsafe performance measurement"
        echo "        => max deviation=$variance%, tolerance=$VARIANCE_ACCEPTED%"
        WARNING_VARIANCE="$WARNING_VARIANCE\n$benchcomputed: max deviation=$variance%, tolerance=$VARIANCE_ACCEPTED%"
        #else
        # echo "[INFO] Maximal deviation from arithmetic mean of 3 average runs: $variance%"
    fi
    PROCESSED_TIME="$time"
    rm -f avg.out
}

run_bench() {
    $1 >____tempfile.data.polybench
    $1 >>____tempfile.data.polybench
    $1 >>____tempfile.data.polybench
    $1 >>____tempfile.data.polybench
    $1 >>____tempfile.data.polybench

    compute_mean_exec_time "____tempfile.data.polybench" "$1"

    rm -f ____tempfile.data.polybench
}

if [[ $# -ne 2 ]]; then
    echo "usage: $0 [output_file] [title]"
    exit 1
fi

OUTFILE=$1
TITLE=$2

echo "Generating data file $OUTFILE"
echo -e "\"Bench name\" Pluto Atiling \"Gain $TITLE\"" >$OUTFILE

for bench in "${BENCHMARK_TESTS[@]}"; do
    OLDIFS=$IFS
    IFS=';'
    set -- $bench
    IFS=$OLDIFS

    base="./$1"
    pluto="${base}_pluto"
    atiling="${base}_atiling"
    bench_name=$(basename $base)

    base_score=0
    pluto_score=0
    atiling_score=0
    gain_over_pluto=0

    echo "---- Bench $bench_name"
    # echo "---- Default"
    # run_bench $base
    # base_score=$PROCESSED_TIME
    # echo "-------- Time $PROCESSED_TIME"

    echo "------ Pluto"
    run_bench $pluto
    pluto_score=$PROCESSED_TIME
    # pluto_gain=$(echo "scale=5;(($base_score - $pluto_score) / $base_score) * 100" | bc)

    echo "-------- Time $pluto_score"

    echo "------ Atiling"
    run_bench $atiling
    atiling_score=$PROCESSED_TIME
    # atiling_gain=$(echo "scale=5;(($base_score - $atiling_score) / $base_score) * 100" | bc)
    gain_over_pluto=$(echo "scale=5;(($pluto_score - $atiling_score) / $pluto_score) * 100" | bc)

    echo "-------- Time $atiling_score"
    echo "-------- Gain (over pluto) $gain_over_pluto"

    if [[ -n $OUTFILE ]]; then
        echo "$bench_name $pluto_score $atiling_score $gain_over_pluto" >>$OUTFILE
    fi

done
