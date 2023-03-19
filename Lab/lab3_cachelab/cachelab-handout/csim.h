#ifndef CSIM_H
#define CSIM_H

typedef unsigned int uint;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned long uint64_t;


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
    uint param_s;
    uint paramS;
    uint paramE;
    uint param_b;
    uint paramB;
    uint64_t setIndexMask;
    uint64_t blockOffsetMask;
    uint64_t tagMask;
}cache_t;

#endif