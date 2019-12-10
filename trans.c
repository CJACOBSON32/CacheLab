/*
 * Names: Cameron Jacobson & Winnie Ly
 *
 */
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include "cachelab.h"

void transpose32(int M, int N, int A[N][M], int B[M][N]);

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	if (N == 32) {

		transpose32(M, N, A, B);

	} else if (N == 64) {

		transpose64(M, N, A, B);

	} else {

		tranposeOther(M, N, A, B);

	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, tmp;

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			tmp = A[i][j];
			B[j][i] = tmp;
		}
	}

}

/*
 * transpose32 - A transpose function for a matrix of size 32x32
 */
char trans_desc32[] = "Transpose of a 32x32 matrix";
void transpose32(int M, int N, int A[N][M], int B[M][N]) {

	int i, j, blockRow, blockCol, diagonal, temp;
	int blockSection = 8;

	for (i = 0; i < N; i+= blockSection) {

		for (j = 0; j < M; j += blockSection) {

			for (blockRow = i; blockRow < i+blockSection; blockRow++) {

				for (blockCol = j; blockCol < j+blockSection; blockCol++) {

					if (blockRow == blockCol) {

						temp = A[blockRow][blockCol];
						diagonal = blockRow;

					} else {

						B[blockCol][blockRow] = A[blockRow][blockCol];
					}
				}

				if (i == j) {

					B[diagonal][diagonal] = temp;

				}
			}
		}
	}
}
/*
 * transpose64 -  A transpose function for a matrix of size 64x64
 *
 */
char trans_desc64[] = "Transpose of a 64x64 matrix";
void transpose64(int M, int N, int A[M][N], int B[M][N]) {

	int i, j, k, l, block0, block1, block2, block3, block4, block5, block6, block7;

	for (i = 0; i < N; i += 8) {

		for (j = 0; j < M; j += 8) {

			//First for loop goes through the top half
			for (k = i; k < i + 4; k++) {

				block0 = A[k][j];
				block1 = A[k][j + 1];
				block2 = A[k][j + 2];
				block3 = A[k][j + 3];
				block4 = A[k][j + 4];
				block5 = A[k][j + 5];
				block6 = A[k][j + 6];
				block7 = A[k][j + 7];

				//Transposing every 4 blocks
				B[j][k] = block0;
				B[j + 1][k] = block1;
				B[j + 2][k] = block2;
				B[j + 3][k] = block3;
				B[j][k + 4] = block4;
				B[j + 1][k + 4] = block5;
				B[j + 2][k + 4] = block6;
				B[j + 3][k + 4] = block7;
			}

			//Second inner for loop goes through the bottom half
			for (l = j + 4; l < j + 8; l++) {

				block4 = A[i + 4][l - 4];
				block5 = A[i + 5][l - 4];
				block6 = A[i + 6][l - 4];
				block7 = A[i + 7][l - 4];

				block0 = B[l - 4][i + 4];
				block1 = B[l - 4][i + 5];
				block2 = B[l - 4][i + 6];
				block3 = B[l - 4][i + 7];

				B[l - 4][i + 4] = block4;
				B[l - 4][i + 5] = block5;
				B[l - 4][i + 6] = block6;
				B[l - 4][i + 7] = block7;

				//Transposing every 4 blocks
				B[l][i] = block0;
				B[l][i + 1] = block1;
				B[l][i + 2] = block2;
				B[l][i + 3] = block3;

				B[l][i + 4] = A[i + 4][l];
				B[l][i + 5] = A[i + 5][l];
				B[l][i + 6] = A[i + 6][l];
				B[l][i + 7] = A[i + 7][l];

			}
		}
	}
}
/*
 * transposeOther - A transpose function for a matrix with an unknown size for M and N
 *
 */
void transposeOther(int M, int N, int A[M][N], int B[M][N]) {


}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
	registerTransFunction(transpose_submit, transpose_submit_desc);

	/* Register any additional transpose functions */
	registerTransFunction(trans, trans_desc);

	/* Register solution for transpose 32x32 matrix*/
	registerTransFunction(transpose32, trans_desc32);

	/* Register solution for transpose 64x64 matrix*/
	registerTransFunction(transpose64, trans_desc64);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
	int i, j;

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; ++j) {
			if (A[i][j] != B[j][i]) {
				return 0;
			}
		}
	}
	return 1;
}

