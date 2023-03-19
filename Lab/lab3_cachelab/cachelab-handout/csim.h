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

#endif