#include "bench.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NMAX 2048

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

double t_start, t_end;

int main() {
	int i, j, k;

	for (i = 0; i < NMAX; i++) {
		for (j = 0; j < NMAX; j++) {
			c[i][j] = 0.0;
			a[i][j] = 1.0 + i * j * 32.5 / (NMAX * NMAX);
			b[i][j] = 1.0 + i * j * 3.4565 / (NMAX * NMAX);
		}
	}

	IF_TIME(t_start = rtclock());

#pragma trahrhe atiling(ATILING_DIV1, ATILING_DIV2, ATILING_DIV3)
	for (i = 0; i < NMAX; i++) {
		for (j = 0; j < NMAX; j++) {
			for (k = j; k < NMAX; k++) {
				c[j][k] += a[i][j] * b[i][k] + b[i][j] * a[i][k];
			}
		}
	}
#pragma endtrahrhe

	IF_TIME(t_end = rtclock());
	IF_TIME(fprintf(stdout, "%0.6lf\n", t_end - t_start));

	return 0;
}
