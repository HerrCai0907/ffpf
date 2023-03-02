#ifndef FFPF_INSTANCE
#define FFPF_INSTANCE

#include "fifo.h"

typedef void (*CallbackOnInt)(uint32_t);
typedef void (*CallbackOnPointer)(void const *);
typedef void (*CallbackOnBytes)(void const *);

struct Instance {
  const struct Fifo *in;
  struct Fifo *out;
  CallbackOnInt onInt;
  CallbackOnPointer onPointer;
  CallbackOnBytes onBytes;
  struct PendingAck {
    void const *ptr;
    CallbackAck callback;
  } pendingAck;
};

void ffpfInitInstancePair(struct Instance *pInsA, struct Instance *pInsB, struct Fifo *fifo1, struct Fifo *fifo2);

bool ffpfSendInt(struct Instance *pIns, uint32_t value);
bool ffpfSendPointer(struct Instance *pIns, const void *value);
bool ffpfSendBytes(struct Instance *pIns, const void *value, CallbackAck callback);

void ffpfSetOnReadInt(struct Instance *pIns, CallbackOnInt callback);
void ffpfSetOnReadPointer(struct Instance *pIns, CallbackOnPointer callback);
void ffpfSetOnReadBytes(struct Instance *pIns, CallbackOnBytes callback);

bool ffpfRead(struct Instance *pIns);

#endif
