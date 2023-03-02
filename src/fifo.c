#include "fifo.h"

void ffpfInitFifo(struct Fifo *const pFifo, struct FifoReader *const pReader, struct FifoWriter *const pWriter) {
  pReader->readerIndex = 0;
  pWriter->writerIndex = 0;
  pFifo->pReader = pReader;
  pFifo->pWriter = pWriter;
}

bool ffpfWriteDataToFifo(struct Fifo *const pFifo, void const *const pWrittenData) {
  bool result;
  struct FifoWriter *const pWriter = pFifo->pWriter;
  struct FifoReader const *const pReader = pFifo->pReader;
  memoryfence;
  if ((pWriter->writerIndex - pReader->readerIndex) < FFPF_FIFO_SIZE) {
    pFifo->apData[pWriter->writerIndex % FFPF_FIFO_SIZE] = pWrittenData;
    memoryfence;
    pWriter->writerIndex++;
    result = true;
  } else {
    /* full */
    result = false;
  }
  return result;
}

bool ffpfReadDataFromFifo(struct Fifo *const pFifo, void const **const ppReadedData) {
  bool result;
  struct FifoWriter const *const pWriter = pFifo->pWriter;
  struct FifoReader *const pReader = pFifo->pReader;
  memoryfence;
  if ((pWriter->writerIndex - pReader->readerIndex) > 0U) {
    *ppReadedData = pFifo->apData[pReader->readerIndex % FFPF_FIFO_SIZE];
    memoryfence;
    pReader->readerIndex++;
    result = true;
  } else {
    /* empty */
    result = false;
  }
  return result;
}