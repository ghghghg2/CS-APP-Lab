/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
// static int splitBlockRowNum = 8;
// static int splitBlockColElemNum = 8;
void my_trans(int M, int N, int A[N][M], int B[M][N]);
void my_trans64x64(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
   /*if ((M == 64) && (N == 64)) {
        splitBlockRowNum = 4;
        splitBlockColElemNum = 4;
    } else if ((M == 32) && (N == 32)){
        splitBlockRowNum = 8;
        splitBlockColElemNum = 8;
    } */
    if ((M == 64) && (N == 64)) {
        my_trans64x64(M, N, A, B);
    } else {
        my_trans(M, N, A, B);
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

// #define DEBUG_MSG
#define splitBlockRowNum 8
#define splitBlockColElemNum 8
#define iterColCnt ((M  / splitBlockColElemNum) + (int)((M % splitBlockColElemNum) > 0))
#define iterRowCnt ((N / splitBlockRowNum) + (int)((N % splitBlockRowNum) > 0))

char my_trans_desc[] = "my transpose";
void my_trans(int M, int N, int A[N][M], int B[M][N])
{
    int baseRowIdx, baseColIdx;
    int tmpDiagData[8];
    int i, j;

    for (baseRowIdx = 0; baseRowIdx < N; baseRowIdx += splitBlockRowNum) {
        for (baseColIdx = 0; baseColIdx < M; baseColIdx += splitBlockColElemNum) {
            #ifdef DEBUG_MSG
            printf("************Block Base Index(%d, %d)************\n", baseRowIdx, baseColIdx);
            #endif
            for (i = 0; i < splitBlockRowNum; i++) {
                if ((baseRowIdx + i) < N) {
                    for (j = 0; j < splitBlockColElemNum; j++) {
                        if ((baseColIdx + j) < M) {
                            tmpDiagData[j] = A[baseRowIdx + i][baseColIdx + j];
                            #ifdef DEBUG_MSG
                                printf("A[%d][%d]: %x\n", baseRowIdx + i, baseColIdx + j, A[baseRowIdx + i][baseColIdx + j]);
                            #endif
                        } else {
                            break;
                        }
                    }
                    for (j = 0; j < splitBlockColElemNum; j++) {
                        if ((baseColIdx + j) < M) {
                            B[baseColIdx + j][baseRowIdx + i] = tmpDiagData[j];
                            #ifdef DEBUG_MSG
                            printf("B[%d][%d]: %x\n", baseColIdx + j, baseRowIdx + i, B[baseColIdx + j][baseRowIdx + i]);
                            #endif
                        } else {
                            break;
                        }
                    }   
                } else {
                    break;
                }
            }
        }
        #ifdef DEBUG_MSG
        #endif
    }
}


#define splitBlockRowNum64 8
#define splitBlockColElemNum64 8
#define iterColCnt64 ((M  / splitBlockColElemNum64) + (int)((M % splitBlockColElemNum64) > 0))
#define iterRowCnt64 ((N / splitBlockRowNum64) + (int)((N % splitBlockRowNum64) > 0))
char my_trans64x64_desc[] = "my transpose 64";
void my_trans64x64(int M, int N, int A[N][M], int B[M][N])
{
    int baseRowIdx, baseColIdx;
    int tmpDiagData[8];
    int i, j;

    for (baseRowIdx = 0; baseRowIdx < N; baseRowIdx += splitBlockRowNum) {
        for (baseColIdx = 0; baseColIdx < M; baseColIdx += splitBlockColElemNum) {
            /* Process (8 * 8) block */
            /* Upper part of A sub-block */
            for (i = 0; i < 4; i++) {
                /* Ipper_left sub-block */
                for (j = 0; j < 4; j++) {
                    tmpDiagData[j] = A[baseRowIdx + i][baseColIdx + j];
                }
                for (j = 0; j < 4; j++) {
                    B[baseColIdx + j][baseRowIdx + i] = tmpDiagData[j];
                }
                /* Upper-Right sub-block */
                for (j = 4; j < 8; j++) {
                    tmpDiagData[j] = A[baseRowIdx + i][baseColIdx + j];
                }
                for (j = 4; j < 8; j++) {
                    B[baseColIdx + j - 4][baseRowIdx + i + 4] = tmpDiagData[j];
                }
            }
            /* Lower part of A sub-block */
            /* Lower-Left sub-block */
            for (j = 0; j < 4; j++) {
                for (i = 4; i < 8; i++) {
                    tmpDiagData[i] = B[baseColIdx + j][baseRowIdx + i];
                    B[baseColIdx + j][baseRowIdx + i] = A[baseRowIdx + i][baseColIdx + j];
                }
                /* Move Upper-Right sub-block of B to Left-Down sub-block of B  */
                for (i = 4; i < 8; i++) {
                    B[baseColIdx + j + 4][baseRowIdx + i - 4] = tmpDiagData[i];
                }
            }
            /* Lower-Right sub-block */
            for (j = 4; j < 8; j++) {
                for (i = 4; i < 8; i++) {
                    tmpDiagData[i] = A[baseRowIdx + i][baseColIdx + j];
                }
                for (i = 4; i < 8; i++) {
                    B[baseColIdx + j][baseRowIdx + i] = tmpDiagData[i];
                }
            }
        }
    }
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
    //registerTransFunction(trans, trans_desc); 
    //registerTransFunction(my_trans, my_trans_desc); 
    //registerTransFunction(my_trans64x64, my_trans64x64_desc); 
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

