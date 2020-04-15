#ifndef GOBACKN_MESSAGE_STRUCT_H
#define GOBACKN_MESSAGE_STRUCT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct GoBackNMessageStruct {
  uint32_t size;  // including these header fields
  int32_t seqNo;
  int32_t seqNoExpected;
  uint32_t crcSum;
  char data[0];
} __attribute__((packed, aligned(1))) GoBackNMessageStruct;

GoBackNMessageStruct* allocateGoBackNMessageStruct(size_t dataSize);
void freeGoBackNMessageStruct(GoBackNMessageStruct* msg);
uint32_t crcGoBackNMessageStruct(GoBackNMessageStruct* msg);

#endif /* GOBACKN_MESSAGE_STRUCT_H */
