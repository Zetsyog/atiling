
#include <stdio.h>

#define N 200
#define M 200

int main(void) {
	int alpha = 5, div1 = 8;
	float A[N][M] = {0}, B[N][M] = {0}, C[N][M] = {0};
	int i, j, k;

#pragma trahrhe atiling(div1, 16, 32)
	for (i = 0; i < N; i++)
		for (j = 0; j <= i; j++) {
			for (k = 0; k < M; k++) {
				C[i][j] +=
					A[j][k] * alpha * B[i][k] + B[j][k] * alpha * A[i][k];
			}
		}
#pragma endtrahrhe
	return 0;
}
