#include "csim.h"
#include "stdlib.h"
#include "stdio.h"

extern const bool configShowVerbose;
extern uint missCnt;
extern uint hitCnt;
extern uint evictionCnt;

void hit_callback(void) __attribute__((weak, alias("default_callback")));
void miss_callback(void) __attribute__((weak, alias("default_callback")));
void eviction_callback(void) __attribute__((weak, alias("default_callback")));

void default_callback(void)
{
    /* Blank */
    printf("Please implement callback.\n");
}

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
    if (pCache->cacheSetArr == NULL)
    {
        printf("No enough space to allocate.\n");
        exit(0);
    }
}

void cacheProcess(const cache_t *pCache, const memOpInfo_t *pMemOpInfo)
{
    uint64_t addr = pMemOpInfo->addr;
    uint64_t setIdx = (addr & pCache->setIndexMask) >> (pCache->param_b);
    uint64_t tag = (addr & pCache->tagMask) >> (pCache->param_b + pCache->param_s);
    cacheSet_t *pCacheSet;
    listNode_t *pIdxNode, *pTailNode;
    cacheLine_t *pTmpLine;

    /* Find the cache set by set index */
    pCacheSet = &pCache->cacheSetArr[setIdx];
    pIdxNode = pCacheSet->lineList.pHeadNode;
    /* Find if the target line in the cache set */
    while (pIdxNode)
    {
        pTmpLine = getCacheLineViaNode(pIdxNode);
        if (tag == pTmpLine->tag)
        {
            /* Hit */
            hit_callback();
            listMoveNodeToHead(pIdxNode);
            break;
        }
        /* Next Node */
        pIdxNode = listGetNextNode(pIdxNode);
    }
    /* Miss */
    if (!pIdxNode)
    {
        /* tag not in the cache set */
        miss_callback();
        if (pCacheSet->lineCnt < pCache->paramE)
        {
            /* More space available for new line */
            pTmpLine = calloc(1, sizeof(cacheLine_t));
            if (pTmpLine)
            {
                pTmpLine->valid = true;
                pTmpLine->tag = tag;
                pCacheSet->lineCnt++;
                listInsertHead(&pCacheSet->lineList, &pTmpLine->lineNode);
            }
            else
            {
                printf("No enough space for cache.\n");
                exit(0);
            }
        }
        else
        {
            /* No more space available for new line */
            eviction_callback();
            /* Find the tail node as LRU node */
            listFindTailNode(&pCacheSet->lineList, pTailNode);
            /* Update the line info */
            pTmpLine = getCacheLineViaNode(pTailNode);
            pTmpLine->tag = tag;
            pTmpLine->valid = true;
            /* move the tail node to head */
            listMoveNodeToHead(pTailNode);
        }
    }
}