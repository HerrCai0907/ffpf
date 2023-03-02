#ifndef FFPF_CONFIG
#define FFPF_CONFIG

#include <stdint.h>

// constant
#define FFPF_FIFO_SIZE 1024U

// datatype
typedef uint32_t FifoIndex;

// builtin function
#define memoryfence __sync_synchronize()

#endif
