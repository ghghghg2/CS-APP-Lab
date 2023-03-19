#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "unistd.h"
#include "getopt.h"
#include "cachelab.h"

typedef unsigned int uint;
typedef unsigned char uint8_t;
typedef char int8_t;

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


int main(int argc, char *argv[])
{
    /* Related to input argument */
    int opt;
    int tmp;
    bool generalInputError = false;
    /* Show verbose or not */
    bool configShowVerbose = false;
    /* Related to reading file */
    FILE *pInputFile = NULL;
    int8_t *pEndTmp;
    int8_t textLine[MAX_LINE_LEN] = {0};
    /* Cache parameters */
    uint cacheParam_S = 0;
    uint cacheParam_E = 0;
    uint cacheParam_B = 0;
    

    /* Input processing */
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
                    cacheParam_S = 1 << tmp;
                } else {
                    generalInputError = true;
                }
            break;
            case 'E': /* num of line */
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    cacheParam_E = tmp;
                } else {
                    generalInputError = true;
                }
            break;
            case 'b': /* block */
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    cacheParam_B = 1 << tmp;
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
    if ((cacheParam_B == 0) || (cacheParam_E == 0) || (cacheParam_S == 0)) {
        generalInputError = true;
    }
    if (generalInputError) {
        printf(helpInfo);
        return 0;
    }

    while(fgets(textLine, MAX_LINE_LEN, pInputFile)) {
        printf("%s", textLine);
    }

    (void)configShowVerbose;

    printSummary(0, 0, 0);
    return 0;
}
