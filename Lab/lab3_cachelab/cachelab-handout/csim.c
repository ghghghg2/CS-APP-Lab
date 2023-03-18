#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "getopt.h"
#include "cachelab.h"

typedef unsigned int uint;
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

int main(int argc, char *argv[])
{
    int opt;
    int tmp;
    int8_t *pEndTmp;
    uint cacheParam_S;
    uint cacheParam_E;
    uint cacheParam_B;

    
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h': /* help */
                printf(helpInfo);
            break;
            case 'v':
            break;
            case 's':
                tmp = strtol(optarg, &pEndTmp, 10);
                if ((pEndTmp != optarg) && (tmp > 0)) {
                    cacheParam_S = 1 << tmp;
                } else {
                    printf(helpInfo);
                }
            break;
            case 'E':
            break;
            case 'b':
            break;
            case 't':
            break;
            default:
            break;
        }
    }
    (void)cacheParam_B;
    (void)cacheParam_E;
    (void)cacheParam_S;

    printSummary(0, 0, 0);
    return 0;
}
