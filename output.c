
#include <stdio.h>

int main(void) {
	int i, j, k, alpha, N, M, div1;
	float A[N][M], B[N][M], C[N][M], D[N][N][M];

	// begin atiling
	long int i_pcmax	 = i_Ehrhart(N, M);
	long int TILE_VOL_L1 = i_pcmax / div1;
	long int ubit		 = max(i_pcmax / (TILE_VOL_L1)-1, 0);

#pragma omp parallel
	for (long int it = 0; it <= ubit; it++) {
		long int lbi = trahrhe_i(max(it * (TILE_VOL_L1 + 1), 1), N, M);
		long int ubi =
			trahrhe_i(min((it + 1) * (TILE_VOL_L1 + 1), i_pcmax), N, M);

		long int k_pcmax	 = k_Ehrhart(N, M, lbi, ubi);
		long int TILE_VOL_L3 = k_pcmax / 32;
		long int ubkt		 = max(k_pcmax / (TILE_VOL_L3)-1, 0);

		for (long int kt = 0; kt <= ubkt; kt++) {
			long int lbk =
				trahrhe_k(max(kt * (TILE_VOL_L3 + 1), 1), N, M, lbi, ubi);
			long int ubk = trahrhe_k(min((kt + 1) * (TILE_VOL_L3 + 1), k_pcmax),
									 N, M, lbi, ubi);

			for (long int i = max(0, lbi); i <= min(0 + 1 * N + -1 * 1, ubi);
				 i++) {
				for (long int j = 0; j <= 0 + 1 * i; j++) {
					B[i][j] += 2 * alpha;
					for (long int k = max(0, lbk);
						 k <= min(0 + 1 * M + -1 * 1, ubk); k++) {
						C[i][j] += A[j][k] * alpha * B[i][k] +
								   B[j][k] * alpha * A[i][k];
						D[i][j][k] = alpha * A[j][k];
					}
				}
			}
		}
	}
	// end atiling

	exit(1);
}
