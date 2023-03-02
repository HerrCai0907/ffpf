#include "fifo.h"

void ffpfInitFifo(struct Fifo *pFifo, struct FifoReader *pReader, struct FifoWriter *pWriter) {
  pReader->readerIndex = 0;
  pWriter->writerIndex = 0;
  pFifo->pReader = pReader;
  pFifo->pWriter = pWriter;
}

// cppcheck-suppress misra-c2012-19.2 // variant record
bool ffpfWriteDataToFifo(struct Fifo *pFifo, const union DataPack *pWrittenData) {
  bool result;
  struct FifoWriter *const pWriter = pFifo->pWriter;
  struct FifoReader const *const pReader = pFifo->pReader;
  memoryfence;
  if ((pWriter->writerIndex - pReader->readerIndex) < FFPF_FIFO_SIZE) {
    pFifo->aData[pWriter->writerIndex % FFPF_FIFO_SIZE] = *pWrittenData;
    memoryfence;
    pWriter->writerIndex++;
    result = true;
  } else {
    /* full */
    result = false;
  }
  return result;
}

// cppcheck-suppress misra-c2012-19.2 // variant record
bool ffpfReadDataFromFifo(struct Fifo const *pFifo, union DataPack *pReadedData) {
  bool result;
  struct FifoWriter const *const pWriter = pFifo->pWriter;
  struct FifoReader *const pReader = pFifo->pReader;
  memoryfence;
  if ((pWriter->writerIndex - pReader->readerIndex) > 0U) {
    *pReadedData = pFifo->aData[pReader->readerIndex % FFPF_FIFO_SIZE];
    memoryfence;
    pReader->readerIndex++;
    result = true;
  } else {
    /* empty */
    result = false;
  }
  return result;
}