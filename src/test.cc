#include <fmt/core.h>
#include <fmt/ranges.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "boot_record.h"
#include "example_disk.h"

namespace {

void PrintHexData(std::span<const std::byte> data) {
  const int bytes_per_line = 16;
  const int row_prefix_width = 4 + 1;
  std::cout << std::string(row_prefix_width, ' ');
  for (int i = 0; i < bytes_per_line; ++i) {
    if (i % 2 == 0) {
      fmt::print(" ");
    }
    fmt::print("{:02x}", i);
  }
  for (int i = 0; i < data.size(); ++i) {
    if (i % bytes_per_line == 0) {
      fmt::print("\n{:04x}:", i);
    }
    if (i % 2 == 0) {
      fmt::print(" ");
    }
    fmt::print("{:02x}", data[i]);
  }
  std::cout << std::endl;
}

TEST(ExampleDisk, Size) { EXPECT_EQ(16 * 1024 * 1024, example_disk.size()); }

TEST(ExampleDisk, BootRecord) {
  BootRecord mbr;
  std::memcpy(&mbr, example_disk.data(), sizeof(mbr));

  EXPECT_EQ(mbr.partitions[0].status, 0x80);
  EXPECT_EQ(
      mbr.partitions[0].chs_start,
      (ChsAddress{.head = 0, .sector = 1, .cylinder_hi = 0, .cylinder_lo = 0}));
  EXPECT_EQ(mbr.boot_signature, 0xAA'55);

  for (const auto& p : mbr.partitions) {
    std::cout << p << std::endl;
  }
}

}  // namespace
