#include "DataBuffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>

typedef struct DataBufferHead {
  long minSeqNo;
  size_t maxCount;
  size_t firstIndex, freeIndex;
  size_t count;
  DataPacket** data;
} DataBufferHead;

DataBuffer allocateDataBuffer(size_t maxPacketCount) {
  DataBufferHead* head = (DataBufferHead*)malloc(sizeof(*head));
  head->minSeqNo = 0;
  head->maxCount = maxPacketCount;
  head->firstIndex = head->freeIndex = head->count = 0;
  head->data = (DataPacket**)calloc(head->maxCount, sizeof(DataPacket*));

  return head;
}

void deallocateDataBuffer(DataBuffer buffer) {
  if (buffer->count > 0) {
    freeBuffer(buffer, buffer->minSeqNo, buffer->minSeqNo + buffer->count - 1);
  }
  free(buffer->data);
  free(buffer);
}

long getFirstSeqNoOfBuffer(DataBuffer buffer) { return buffer->minSeqNo; }

long getLastSeqNoOfBuffer(DataBuffer buffer) {
  return buffer->minSeqNo + buffer->count - 1;
}

size_t getBufferSize(DataBuffer buffer) { return buffer->count; }

bool bufferContainsPacket(DataBuffer buffer, long seqNo) {
  seqNo -= buffer->minSeqNo;
  return seqNo >= 0 && seqNo < buffer->count;
}

DataPacket* getDataPacketFromBuffer(DataBuffer buffer, long seqNo) {
  if (seqNo < buffer->minSeqNo || seqNo >= buffer->minSeqNo + buffer->count) {
    return NULL;
  }
  return buffer->data
      [(seqNo - buffer->minSeqNo + buffer->firstIndex) % buffer->maxCount];
}

void putDataPacketIntoBuffer(DataBuffer buffer, DataPacket* data) {
  assert(buffer->count < buffer->maxCount);
  assert(data->packet->seqNo >= buffer->minSeqNo);
  assert(data->packet->seqNo < buffer->minSeqNo + buffer->maxCount);
  assert((data->packet->seqNo - buffer->minSeqNo + buffer->firstIndex) %
             buffer->maxCount ==
         buffer->freeIndex);

  buffer->data[buffer->freeIndex++] = data;
  buffer->freeIndex %= buffer->maxCount;
  ++buffer->count;
}

void freeBuffer(DataBuffer buffer, long start, long end) {
  assert(end >= start);
  assert(start == buffer->minSeqNo);
  assert(end - start <= buffer->count);

  for (long i = start; i <= end; ++i) {
    assert(i == buffer->minSeqNo);

    free(buffer->data[buffer->firstIndex]->packet);
    free(buffer->data[buffer->firstIndex++]);
    buffer->firstIndex %= buffer->maxCount;
    ++buffer->minSeqNo;
    --buffer->count;
  }
}

void printBuffer(DataBuffer buffer) {
  printf("%u packets:\n", (unsigned int)buffer->count);

  for (size_t i = buffer->firstIndex; i != buffer->freeIndex;
       i = (i + 1) % buffer->maxCount) {
    GoBackNMessageStruct* msg = buffer->data[i]->packet;

    printf("%" PRId32 ": %" PRIu32 " data bytes (CRC: %" PRIu32 ").\n",
           msg->seqNo, msg->size, msg->crcSum);
  }
}

void resetTimers(DataBuffer buffer) {
  for (size_t i = buffer->firstIndex; i != buffer->freeIndex;
       i = (i + 1) % buffer->maxCount) {
    buffer->data[i]->timeout.tv_sec = LONG_MAX;
    buffer->data[i]->timeout.tv_usec = 0;
  }
}
