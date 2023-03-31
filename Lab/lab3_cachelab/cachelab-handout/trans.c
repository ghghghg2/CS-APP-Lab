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

void my_trans(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    my_trans(M, N, A, B);

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


#define splitBlockRowNum (8)
#define splitBlockColElemNum (8)


char my_trans_desc[] = "my transpose";
void my_trans(int M, int N, int A[N][M], int B[M][N])
{
    int iterColResidual = M % splitBlockColElemNum;
    int iterColCnt = (M  / splitBlockColElemNum) + (int)(iterColResidual > 0);
    int iterRowResidual = N % splitBlockRowNum;
    int iterRowCnt = (N / splitBlockRowNum) + (int)(iterRowResidual > 0);
    int baseRowIdx, baseColIdx;

    int blockRowIdx, blockColIdx;
    int i, j, tmp;

    
    for (blockRowIdx = 0; blockRowIdx < iterRowCnt; blockRowIdx++) {
        baseRowIdx = blockRowIdx * splitBlockRowNum;
        for (blockColIdx = 0; blockColIdx < iterColCnt; blockColIdx++) {
            baseColIdx = blockColIdx * splitBlockColElemNum;
            /* Process a single block */
            #ifdef DBG_VERBOSE
            printf("***Block: (%d, %d)***\n", baseRowIdx, baseColIdx);
            #endif
            for (i = 0; i < splitBlockRowNum; i++) {
                if ((baseRowIdx + i) < N) {
                    /* if the row index doesn't exceed N */
                    for (j = 0; j < splitBlockColElemNum; j++) {
                        if ((baseColIdx + j) < M) {
                            /* if col index doesn't exceed M */ 
                            tmp = A[baseRowIdx + i][j + baseColIdx];
                            B[j + baseColIdx][baseRowIdx + i] = tmp;
                            #ifdef DBG_VERBOSE
                            printf("A[%d][%d] == %x => ", (baseRowIdx + i), (baseColIdx + j), A[baseRowIdx + i][j + baseColIdx]);
                            printf("B[%d][%d] == %x\n", (baseColIdx + j), (baseRowIdx + i), B[j + baseColIdx][baseRowIdx + i]);
                            #endif
                        } else {
                            /* stop if col index exceed M */
                            #ifdef DBG_VERBOSE
                            printf("****Column Exceed: %d****\n", (baseColIdx + j));
                            #endif
                            break;
                        }
                    }
                } else {
                    /* stop if the row index exceed N */
                    #ifdef DBG_VERBOSE
                    printf("****Row Exceed: %d****\n", (baseRowIdx + i));
                    #endif
                    break;
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
    registerTransFunction(trans, trans_desc); 
    registerTransFunction(my_trans, my_trans_desc); 

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

