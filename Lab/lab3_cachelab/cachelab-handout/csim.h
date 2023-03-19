#ifndef CSIM_H
#define CSIM_H

typedef unsigned int uint;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned long uint64_t;

/* Doubly linked List structure */
#define listInsertHead(pList, pNewNode) \
do {\
    listNode_t *pTmpNode; \
    if((pTmpNode = (pList)->pHeadNode) != NULL) { \
        pTmpNode->pPrevNode = (pNewNode); \
    } \
    (pNewNode)->pNextNode = pTmpNode; \
    (pNewNode)->pPrevNode = NULL; \
    (pNewNode)->pList = (pList); \
} while(0)

#define listMoveNodeToHead(pNode) \
do { \
    /* Check if pNode is already head */ \
    list_t *pList = (pNode)->pList; \
    listNode_t *pOriginHead; \
    if ((pOriginHead = pList->pHeadNode) == (pNode)) { \
        /* Do nothing */ \
        break; \
    } \
    /* Connect the prev node to next node */ \
    listNode_t *pTmpPrevNode = (pNode)->pPrevNode; \
    listNode_t *pTmpNextNode = (pNode)->pNextNode; \
    pTmpPrevNode->pNextNode = pTmpNextNode; \
    pTmpNextNode->pPrevNode = pTmpPrevNode; \
    (pNode)->pNextNode = pOriginHead; \
    (pNode)->pPrevNode = NULL; \
    pOriginHead->pPrevNode = (pNode); \
    pList->pHeadNode = (pNode); \
} while(0)

#define getNextNode(pNode) ((pNode)->pNextNode)
#define getPrevNode(pNode) ((pNode)->pPrevNode)
#define getOwnerPtr(pNode, ownerType_t, nodeMemberName) \
((ownerType_t *)((void *)(pNode) - (void *)(&(ownerType_t*)(0)->nodeMemberName)))

typedef struct listNode_t{
    struct listNode_t *pPrevNode;
    struct listNode_t *pNextNode;
    void *pList;
}listNode_t;

typedef struct {
    listNode_t *pHeadNode;
}list_t;


/* Cache structure */

typedef struct {
    list_t lineList;
    uint lineCnt;
}cacheSet_t;

typedef struct {
    bool valid;
    uint64_t tagVal;
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

#endif