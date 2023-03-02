#ifndef FFPF_FIFO
#define FFPF_FIFO

#include "config.h"
#include <stdbool.h>

struct FifoReader;
struct FifoWriter;

struct DataPackCommon {
  enum DataType {
    INT,
    POINTER,
    BYTES,
    ACK,
  } type;
};
struct DataPackInt {
  struct DataPackCommon common;
  uint32_t value;
};
struct DataPackPointer {
  struct DataPackCommon common;
  void const *value;
};
typedef void (*CallbackAck)(void const *);
struct DataPackBytes {
  struct DataPackCommon common;
  void const *ptr;
  CallbackAck callback;
};

// cppcheck-suppress misra-c2012-19.2 // variant record
union DataPack {
  struct DataPackCommon common;
  struct DataPackInt i;
  struct DataPackPointer p;
  struct DataPackBytes b;
};

struct Fifo {
  struct FifoReader *pReader;
  struct FifoWriter *pWriter;
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack aData[FFPF_FIFO_SIZE];
};

struct FifoReader {
  FifoIndex readerIndex;
};

struct FifoWriter {
  FifoIndex writerIndex;
};

void ffpfInitFifo(struct Fifo *pFifo, struct FifoReader *pReader, struct FifoWriter *pWriter);
// cppcheck-suppress misra-c2012-19.2 // variant record
bool ffpfWriteDataToFifo(struct Fifo *pFifo, const union DataPack *pWrittenData);
// cppcheck-suppress misra-c2012-19.2 // variant record
bool ffpfReadDataFromFifo(struct Fifo const *pFifo, union DataPack *pReadedData);

#endif
