#ifndef FFPF_FIFO
#define FFPF_FIFO

#include "config.h"
#include <stdbool.h>

struct FifoReader;
struct FifoWriter;

struct DataPackCommon {
  enum DataType {
    INT,
    FLOAT,
    POINTER,
    ACK,
  } type;
};
struct DataPackInt {
  struct DataPackCommon common;
  uint32_t value;
};
struct DataPackFloat {
  struct DataPackCommon common;
  float value;
};
struct DataPackPointer {
  struct DataPackCommon common;
  void const *ptr;
  void (*callback)(void *);
};
struct DataPackAck {
  struct DataPackCommon common;
  void *ptr;
  void (*callback)(void *);
};

// cppcheck-suppress misra-c2012-19.2 // variant record
union DataPack {
  struct DataPackCommon common;
  struct DataPackInt i;
  struct DataPackFloat f;
  struct DataPackPointer p;
};

struct Fifo {
  struct FifoReader *pReader;
  struct FifoWriter *pWriter;
  union DataPack aData[FFPF_FIFO_SIZE];
};

struct FifoReader {
  FifoIndex readerIndex;
};

struct FifoWriter {
  FifoIndex writerIndex;
};

void ffpfInitFifo(struct Fifo *const pFifo, struct FifoReader *const pReader, struct FifoWriter *const pWriter);
bool ffpfWriteDataToFifo(struct Fifo *const pFifo, const union DataPack *const pWrittenData);
bool ffpfReadDataFromFifo(struct Fifo const *const pFifo, union DataPack *const pReadedData);

#endif
