#include "csim.h"
#include "stdlib.h"
#include "stdio.h"

void cacheInitialize(cache_t *pCache, uint param_s, uint paramE, uint param_b)
{
    pCache->param_s = param_s;
    pCache->param_b = param_b;
    pCache->paramB = (1 << param_b);
    pCache->paramS = (1 << param_s);
    pCache->paramE = paramE;
    pCache->blockOffsetMask = pCache->paramB - 1;
    pCache->setIndexMask = (pCache->paramS - 1) << (pCache->param_b);
    pCache->tagMask = ~(pCache->blockOffsetMask | pCache->setIndexMask);
    pCache->cacheSetArr = calloc(pCache->paramS, sizeof(cacheSet_t));
    if (pCache->cacheSetArr == NULL) {
        printf("No enough space to allocate.\n");
        exit(0);
    }
    
}