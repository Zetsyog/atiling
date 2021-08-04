#ifndef ATILING_BENCHMARK_H
#define ATILING_BENCHMARK_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>
#define LARGE_DATASET 1000

double get_time() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 1e-6;
}

double start, elapsed;
#define start_bench(name)                                          \
	do {                                                           \
		fprintf(stdout, "[BENCH] Starting benchmark %s \n", name); \
		start = get_time();                                        \
	} while (0)

#define stop_bench()                  \
	do {                              \
		elapsed = get_time() - start; \
	} while (0)

#define print_bench_result(file)                         \
	do {                                                 \
		fprintf(file, "[BENCH] Done in %fs\n", elapsed); \
	} while (0)

#endif
