#include "bench.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NMAX 2000

#pragma declarations
double a[NMAX][NMAX], b[NMAX][NMAX], c[NMAX][NMAX];
#pragma enddeclarations

#define TIME 1

#ifdef TIME
#define IF_TIME(foo) foo;
#else
#define IF_TIME(foo)
#endif

double rtclock() {
	struct timeval Tp;
	int stat;
	stat = gettimeofday(&Tp, NULL);
	if (stat != 0)
		printf("Error return from gettimeofday: %d", stat);
	return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

int main() {
	int i, j, k;
	double t_start, t_end;

	for (i = 0; i < NMAX; i++) {
		for (j = 0; j < NMAX; j++) {
			c[i][j] = 0.0;
			a[i][j] = b[i][j] = i * j * 0.5 / NMAX;
		}
	}

	IF_TIME(t_start = rtclock());

#pragma trahrhe atiling(ATILING_DIV1, ATILING_DIV2, ATILING_DIV3)
	for (i = 0; i < NMAX; i++) {
		for (j = 0; j < NMAX; j++) {
			for (k = 0; k < j - 1; k++) {
				c[i][k] += a[j][k] * b[i][j];
				c[i][j] += a[j][j] * b[i][j];
			}
			// c[i][j] += a[j][j] * b[i][j];
		}
	}
#pragma endtrahrhe

#pragma omp parallel for private(i, j)
	for (i = 0; i < NMAX; i++) {
		for (j = 0; j < NMAX; j++) {
			c[i][j] += a[j][j] * b[i][j];
		}
	}

	IF_TIME(t_end = rtclock());
	IF_TIME(fprintf(stdout, "%0.6lf\n", t_end - t_start));

	// 	if (fopen(".test", "r")) {
	// #ifdef MPI
	// 		if (my_rank == 0) {
	// #endif
	// 			for (i = 0; i < NMAX; i++) {
	// 				for (j = 0; j < NMAX; j++) {
	// 					fprintf(stderr, "%lf ", c[i][j]);
	// 				}
	// 				fprintf(stderr, "\n");
	// 			}
	// #ifdef MPI
	// 		}
	// #endif
	// }

	return 0;
}
