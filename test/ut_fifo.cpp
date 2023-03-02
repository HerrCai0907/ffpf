#include <chrono>
#include <gtest/gtest.h>
#include <thread>
extern "C" {
#include "src/fifo.h"
}

struct FifoTest : public ::testing::Test {
  FifoTest() { ffpfInitFifo(&fifo_, &reader_, &writer_); }

  struct Fifo fifo_;
  struct FifoReader reader_;
  struct FifoWriter writer_;
};

TEST_F(FifoTest, WriteUntillFullAndReadUntilEmpty) {
  std::vector<void *> apData{};
  for (uint32_t i = 0; i < FFPF_FIFO_SIZE; i++) {
    auto v = malloc(100);
    apData.push_back(v);
    auto result = ffpfWriteDataToFifo(&fifo_, v);
    ASSERT_TRUE(result);
  }
  auto fullResult = ffpfWriteDataToFifo(&fifo_, nullptr);
  ASSERT_FALSE(fullResult);

  void const *pData;
  for (uint32_t i = 0; i < FFPF_FIFO_SIZE; i++) {
    auto v = malloc(100);
    apData.push_back(v);
    auto result = ffpfReadDataFromFifo(&fifo_, &pData);
    ASSERT_TRUE(result);
    ASSERT_EQ(pData, apData[i]);
  }
  auto emptyResult = ffpfReadDataFromFifo(&fifo_, &pData);
  ASSERT_FALSE(emptyResult);

  for (auto pData : apData) {
    free(pData);
  }
}

static void testParalled(FifoTest *framework, std::size_t size, bool delayWriter, bool delayReader) {
  std::vector<void *> apWrittenData{};
  std::vector<void const *> apReadedData{};
  std::thread writeThread([framework, size, delayWriter, &apWrittenData] {
    while (apWrittenData.size() < size) {
      auto v = malloc(1);
      bool result = false;
      while (!result) {
        if (delayWriter) {
          std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        result = ffpfWriteDataToFifo(&framework->fifo_, v);
      }
      apWrittenData.push_back(v);
    }
  });
  std::thread readThread([framework, size, delayReader, &apReadedData] {
    void const *pData;
    while (apReadedData.size() < size) {
      bool result = false;
      while (!result) {
        if (delayReader) {
          std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        result = ffpfReadDataFromFifo(&framework->fifo_, &pData);
      }
      apReadedData.push_back(pData);
    }
  });

  writeThread.join();
  readThread.join();

  for (std::size_t i = 0; i < apReadedData.size(); i++) {
    ASSERT_EQ(apReadedData[i], apWrittenData[i]);
  }
  for (auto pData : apWrittenData) {
    free(pData);
  }
}

TEST_F(FifoTest, ParallelReadAndWrite) { testParalled(this, 100000U, false, false); }

TEST_F(FifoTest, FastReadSlowWrite) { testParalled(this, 100000U, true, false); }

TEST_F(FifoTest, FastWriteSlowRead) { testParalled(this, 100000U, false, true); }
