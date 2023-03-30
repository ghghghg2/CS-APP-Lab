#ifndef CSIM_H
#define CSIM_H
#include "stdbool.h"

typedef unsigned int uint;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned long uint64_t;

/* Doubly linked List structure */
#define listGetNextNode(pCurrNode) ((pCurrNode)->pNextNode)

#define listInsertHead(pList_, pNewNode) \
do {\
    listNode_t *pTmpNode_; \
    if((pTmpNode_ = (pList_)->pHeadNode) != NULL) { \
        pTmpNode_->pPrevNode = (pNewNode); \
    } \
    (pNewNode)->pNextNode = pTmpNode_; \
    (pNewNode)->pPrevNode = NULL; \
    (pNewNode)->pList = (pList_); \
    (pList_)->pHeadNode = (pNewNode); \
} while(0)

#define listMoveNodeToHead(pNode) \
do { \
    /* Check if pNode is already head */ \
    list_t *pList_ = (pNode)->pList; \
    listNode_t *pOriginHead_ = pList_->pHeadNode; \
    if (pOriginHead_ == (pNode)) { \
        /* Do nothing */ \
        break; \
    } \
    /* Connect the prev node to next node */ \
    listNode_t *pTmpPrevNode_ = (pNode)->pPrevNode; \
    listNode_t *pTmpNextNode_ = (pNode)->pNextNode; \
    pTmpPrevNode_->pNextNode = pTmpNextNode_; \
    if (pTmpNextNode_) { \
        pTmpNextNode_->pPrevNode = pTmpPrevNode_; \
    }\
    (pNode)->pNextNode = pOriginHead_; \
    (pNode)->pPrevNode = NULL; \
    pOriginHead_->pPrevNode = (pNode); \
    pList_->pHeadNode = (pNode); \
} while(0)

#define listFindTailNode(pList, pTailNode) \
do { \
    listNode_t *pTmpNode_ = (pList)->pHeadNode; \
    if (!pTmpNode_) { \
        (pTailNode) = NULL;\
        break; \
    } \
    while(pTmpNode_->pNextNode) { \
        pTmpNode_ = pTmpNode_->pNextNode; \
    } \
    (pTailNode) = pTmpNode_; \
} while(0)

#define getNextNode(pNode) ((pNode)->pNextNode)
#define getPrevNode(pNode) ((pNode)->pPrevNode)
#define getOwnerPtr(pNode, ownerType_t, nodeMemberName) \
((ownerType_t *)((void *)(pNode) - (void *)(&((ownerType_t*)(0))->nodeMemberName)))

typedef struct listNode_t{
    struct listNode_t *pPrevNode;
    struct listNode_t *pNextNode;
    void *pList;
}listNode_t;

typedef struct {
    listNode_t *pHeadNode;
}list_t;


/* Cache structure */
#define getCacheLineViaNode(pNode) getOwnerPtr(pNode, cacheLine_t, lineNode)

typedef struct {
    list_t lineList;
    uint lineCnt;
}cacheSet_t;

typedef struct {
    bool valid;
    uint64_t tag;
    listNode_t lineNode;
}cacheLine_t;

typedef enum {
    INSTRUCTION = 'I', 
    DATALOAD = 'L',
    DATASTORE = 'S',
    DATAMODIFY = 'M'
}memOpType_t;

typedef struct {
    memOpType_t memOpType;
    uint64_t addr;
    uint8_t size;
}memOpInfo_t;

typedef struct {
    /* Input parameters */
    uint param_s;
    uint paramS;
    uint paramE;
    uint param_b;
    uint paramB;
    uint64_t setIndexMask;
    uint64_t blockOffsetMask;
    uint64_t tagMask;
    /* Simulate parameters */
    cacheSet_t *cacheSetArr;
}cache_t;




void cacheInitialize(cache_t *pCache, uint param_s, uint paramE, uint param_b);

void cacheProcess(const cache_t *pCache, const memOpInfo_t *pMemOpInfo);

#endif