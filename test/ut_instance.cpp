#include <chrono>
#include <gtest/gtest.h>
#include <thread>
extern "C" {
#include "src/instance.h"
}

struct InstanceTest : public ::testing::Test {
  InstanceTest() {
    ffpfInitFifo(&fifoA_, &readerA_, &writerA_);
    ffpfInitFifo(&fifoB_, &readerB_, &writerB_);

    ffpfInitInstancePair(&ins1_, &ins2_, &fifoA_, &fifoB_);
  }

  struct Instance ins1_;
  struct Instance ins2_;

private:
  struct Fifo fifoA_;
  struct FifoReader readerA_;
  struct FifoWriter writerA_;
  struct Fifo fifoB_;
  struct FifoReader readerB_;
  struct FifoWriter writerB_;
};

const uint32_t VALUE = 1024;
uint32_t readIntCount = 0;
TEST_F(InstanceTest, SendInt) {
  ffpfSendInt(&ins1_, VALUE);

  ffpfSetOnReadInt(&ins2_, [](uint32_t value) {
    readIntCount++;
    ASSERT_EQ(value, VALUE);
  });

  auto result = ffpfRead(&ins2_);
  ASSERT_TRUE(result);

  ASSERT_FALSE(ffpfRead(&ins2_));
  ASSERT_EQ(readIntCount, 1);
}

const void *PTR = reinterpret_cast<void *>(2048);
uint32_t readPointerCount = 0;
TEST_F(InstanceTest, SendPointer) {
  ffpfSendPointer(&ins1_, PTR);

  ffpfSetOnReadPointer(&ins2_, [](void const *ptr) {
    readPointerCount++;
    ASSERT_EQ(ptr, PTR);
  });

  auto result = ffpfRead(&ins2_);
  ASSERT_TRUE(result);

  ASSERT_FALSE(ffpfRead(&ins2_));
  ASSERT_EQ(readPointerCount, 1);
}

class ST {
public:
  static uint32_t newCount;
  static uint32_t deleteCount;
  ST() { newCount++; }
  ~ST() { deleteCount++; }
};
uint32_t ST::newCount = 0;
uint32_t ST::deleteCount = 0;

const void *OBJ = new ST();
TEST_F(InstanceTest, SendBytes) {
  ffpfSendBytes(&ins1_, OBJ, [](void const *bytes) { delete static_cast<ST const *>(bytes); });
  ffpfSetOnReadBytes(&ins2_, [](void const *bytes) { ASSERT_EQ(bytes, OBJ); });

  bool result;

  result = ffpfRead(&ins2_);
  ASSERT_TRUE(result);
  result = ffpfRead(&ins2_);
  ASSERT_FALSE(result);

  ASSERT_EQ(ST::deleteCount, 0); // OBJ not delete now

  result = ffpfRead(&ins1_);
  ASSERT_TRUE(result);

  ASSERT_EQ(ST::deleteCount, 1); // OBJ delete now

  result = ffpfRead(&ins1_);
  ASSERT_FALSE(result);
}
