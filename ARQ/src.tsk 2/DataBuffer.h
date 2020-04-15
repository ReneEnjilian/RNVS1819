#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <sys/time.h>
#include "GoBackNMessageStruct.h"

// struct DataBufferHead;
typedef struct DataBufferHead* DataBuffer;
typedef struct DataPacket {
  struct timeval timeout;
  GoBackNMessageStruct* packet;
} DataPacket;

DataBuffer allocateDataBuffer(size_t maxPacketCount);
void deallocateDataBuffer(DataBuffer buffer);

long getFirstSeqNoOfBuffer(DataBuffer buffer);
long getLastSeqNoOfBuffer(DataBuffer buffer);
size_t getBufferSize(DataBuffer buffer);
bool bufferContainsPacket(DataBuffer buffer, long seqNo);

DataPacket* getDataPacketFromBuffer(DataBuffer buffer, long seqNo);
void putDataPacketIntoBuffer(DataBuffer buffer, DataPacket* data);
void freeBuffer(DataBuffer buffer, long start, long end);
void printBuffer(DataBuffer buffer);
void resetTimers(DataBuffer buffer);

#endif /* DATA_BUFFER_H */
