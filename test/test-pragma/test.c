
int i, j, k, alpha;
float A[N][M], B[N][M], C[N][M];
#pragma atiling(8, 16, 32)
for (i = 0; i < N; i++)
	for (j = 0; j <= i; j++) {
		B[i][j] += 2 * alpha;
		for (k = 0; k < M; k++)
			C[i][j] += A[j][k] * alpha * B[i][k] + B[j][k] * alpha * A[i][k];
	}
#pragma endatiling
