#include <stdio.h>
#include "GoBackNMessageStruct.h"
#include "CRC.h"

GoBackNMessageStruct* allocateGoBackNMessageStruct(size_t dataSize) {
  size_t size = sizeof(GoBackNMessageStruct) + ((dataSize + 3) & ~0x3);

  GoBackNMessageStruct* msg = (GoBackNMessageStruct*)malloc(size);
  memset(msg, 0, size);
  return msg;
}

void freeGoBackNMessageStruct(GoBackNMessageStruct* msg) { free(msg); }

uint32_t crcGoBackNMessageStruct(GoBackNMessageStruct* msg) {
  uint32_t crc;
  crc32((void*)msg, (size_t)msg->size, &crc);

  return (crc);
}
