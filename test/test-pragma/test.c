
#include <stdio.h>

int main(void) {
	int i, j, k, alpha, N, M, div1;
	float A[N][M], B[N][M], C[N][M], D[N][N][M];

#pragma atiling(div1, 1, 32)
	for (i = 0; i < N; i++)
		for (j = 0; j <= i; j++) {
			B[i][j] += 2 * alpha;
			for (k = 0; k < M; k++) {
				C[i][j] +=
					A[j][k] * alpha * B[i][k] + B[j][k] * alpha * A[i][k];
				D[i][j][k] = alpha * A[j][k];
			}
		}

#pragma endatiling
	exit(1);
}
