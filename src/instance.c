#include "instance.h"
#include <stddef.h>

void ffpfInitInstancePair(struct Instance *pInsA, struct Instance *pInsB, struct Fifo *fifo1, struct Fifo *fifo2) {
  pInsA->onInt = NULL;
  pInsA->onPointer = NULL;
  pInsA->onBytes = NULL;
  pInsA->pendingAck.ptr = NULL;
  pInsA->pendingAck.callback = NULL;
  pInsB->onInt = NULL;
  pInsB->onPointer = NULL;
  pInsB->onBytes = NULL;
  pInsB->pendingAck.ptr = NULL;
  pInsB->pendingAck.callback = NULL;

  pInsA->in = fifo1;
  pInsB->out = fifo1;

  pInsA->out = fifo2;
  pInsB->in = fifo2;
}

bool ffpfSendInt(struct Instance *pIns, uint32_t value) {
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack dataPack;
  dataPack.i.common.type = INT;
  dataPack.i.value = value;
  return ffpfWriteDataToFifo(pIns->out, &dataPack);
}
bool ffpfSendPointer(struct Instance *pIns, const void *value) {
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack dataPack;
  dataPack.p.common.type = POINTER;
  dataPack.p.value = value;
  return ffpfWriteDataToFifo(pIns->out, &dataPack);
}
bool ffpfSendBytes(struct Instance *pIns, void const *value, CallbackAck callback) {
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack dataPack;
  dataPack.b.common.type = BYTES;
  dataPack.b.ptr = value;
  dataPack.b.callback = callback;
  return ffpfWriteDataToFifo(pIns->out, &dataPack);
}

void ffpfSetOnReadInt(struct Instance *pIns, CallbackOnInt callback) { pIns->onInt = callback; }
void ffpfSetOnReadPointer(struct Instance *pIns, CallbackOnPointer callback) { pIns->onPointer = callback; }
void ffpfSetOnReadBytes(struct Instance *pIns, CallbackOnBytes callback) { pIns->onBytes = callback; }

static bool sendAck(struct Instance *pIns, const void *ptr, CallbackAck callback) {
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack dataPack;
  dataPack.b.common.type = ACK;
  dataPack.b.ptr = ptr;
  dataPack.b.callback = callback;
  return ffpfWriteDataToFifo(pIns->out, &dataPack);
}

bool ffpfRead(struct Instance *pIns) {
  // cppcheck-suppress misra-c2012-19.2 // variant record
  union DataPack data;
  bool sendAckResult = true;
  bool readResult = false;

  /* re-sedn pending ACK */
  if ((pIns->pendingAck.ptr != NULL) || (pIns->pendingAck.callback != NULL)) {
    sendAckResult = sendAck(pIns, pIns->pendingAck.ptr, pIns->pendingAck.callback);
  } else {
    /* do nothing */
  }
  if (sendAckResult) {
    readResult = ffpfReadDataFromFifo(pIns->in, &data);
    if (readResult) {
      switch (data.common.type) {
      case INT:
        if (pIns->onInt != NULL) {
          pIns->onInt(data.i.value);
        }
        break;
      case POINTER:
        pIns->onPointer(data.p.value);
        break;
      case BYTES:
        pIns->onBytes(data.b.ptr);
        sendAckResult = sendAck(pIns, data.b.ptr, data.b.callback);
        if (sendAckResult) {
          /* do nothing */
        } else {
          pIns->pendingAck.ptr = data.b.ptr;
          pIns->pendingAck.callback = data.b.callback;
        }
        break;
      case ACK:
        data.b.callback(data.b.ptr);
        break;
      default:
        break;
      }
    } else {
      /* FIFO is empty, do nothing */
    }
  } else {
    /* do nothing */
  }
  return readResult;
}