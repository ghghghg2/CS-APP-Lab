#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "getopt.h"
#include "cachelab.h"
#include "csim.h"

#define helpInfo \
"Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file."

#define MAX_LINE_LEN 100
/*
Trace File overview
“I” denotes an instruction load
“L” a data load
“S” a data store
“M” a data modify (i.e., a data load followed by a data store)
*/
FILE *pInputFile = NULL;
static memOpInfo_t memOpInfo;
static cache_t cacheSimObj;

static void exitFcn(void)
{
    if (pInputFile) {
        fclose(pInputFile);
    }
    if (cacheSimObj.cacheSetArr) {
        free(cacheSimObj.cacheSetArr);
    }
    // printf("\n**Exit Function.**\n");
}



int main(int argc, char *argv[])
{
    /* Related to input argument */
    int opt;
    int tmp;
    uint param_s, paramE, param_b;
    bool generalInputError = false;
    /* Show verbose or not */
    bool configShowVerbose = false;
    /* Related to reading file */
    int8_t *pEndTmp;
    int8_t textLine[MAX_LINE_LEN] = {0};
    /* Line parsing */
    int8_t *pToken;
    uint8_t tokenCnt = 0;

    /* Register an exit function */
    atexit(exitFcn);
    

    /* Input argument processing */
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h': /* help */
                printf(helpInfo);
                return 0;
            break;
            case 'v': /* verbose */
                configShowVerbose = true;
            break;
            case 's': /* set */
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    param_s = tmp;
                } else {
                    generalInputError = true;
                }
            break;
            case 'E': /* num of line */
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    paramE = tmp;
                } else {
                    generalInputError = true;
                }
            break;
            case 'b': /* block */
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    param_b = tmp;
                } else {
                    generalInputError = true;
                }
            break;
            case 't': /* trace file */
                pInputFile = fopen(optarg, "r");
                if (!pInputFile) {
                    generalInputError = true;
                }
            break;
            default:
                generalInputError = true;
                printf(helpInfo);
                return 0;
            break;
        }
    }
    if ((param_s == 0) || (paramE == 0) || (param_b == 0)) {
        generalInputError = true;
    }
    if (generalInputError) {
        printf(helpInfo);
        exit(0);
    }

    cacheInitialize(&cacheSimObj, param_s, paramE, param_b);

    /* Process trace log */
    while(fgets(textLine, MAX_LINE_LEN, pInputFile)) {
        /* Process a single line */
        pToken = strtok(textLine, " ,");
        /* Token parsing */
        tokenCnt = 1;
        while(pToken != NULL) {
            /* Extract 3 tokens in the line */
            if (tokenCnt == 1) {
                memOpInfo.memOpType = (memOpType_t)pToken[0];
                tokenCnt++;
            } else if (tokenCnt == 2) {
                memOpInfo.addr = strtoul(pToken, &pEndTmp, 16);
                if ((optarg == pEndTmp) || (memOpInfo.addr < 0)) {
                    printf("**Invalid Address.**\n");
                    fclose(pInputFile);
                    return 0;
                }
                tokenCnt++;
            } else if (tokenCnt == 3) {
                memOpInfo.size = strtoul(pToken, &pEndTmp, 16);
                if ((optarg == pEndTmp) || (memOpInfo.addr < 0)) {
                    printf("**Invalid size.**\n");
                    fclose(pInputFile);
                    return 0;
                }
                tokenCnt = 0; /* Ignore the unexpect token*/
            } else {
                /* Ignore the unexpect token */
            }
            /* Go on to process next token */
            pToken = strtok(NULL, " ,");
        }
        /* Cache simulate*/

    }

    (void)configShowVerbose;


    
    printSummary(0, 0, 0);
    return 0;
}


