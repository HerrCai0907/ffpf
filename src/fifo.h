#ifndef FFPF_FIFO
#define FFPF_FIFO

#include "config.h"
#include <stdbool.h>

struct FifoReader;
struct FifoWriter;

struct Fifo {
  struct FifoReader *pReader;
  struct FifoWriter *pWriter;
  void const *apData[FFPF_FIFO_SIZE];
};

struct FifoReader {
  FifoIndex readerIndex;
};

struct FifoWriter {
  FifoIndex writerIndex;
};

void ffpfInitFifo(struct Fifo *const pFifo, struct FifoReader *const pReader, struct FifoWriter *const pWriter);
bool ffpfWriteDataToFifo(struct Fifo *const pFifo, void const *const pWrittenData);
bool ffpfReadDataFromFifo(struct Fifo *const pFifo, void const **const ppReadedData);

#endif
