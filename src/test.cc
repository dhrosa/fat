#include <fmt/core.h>
#include <fmt/ranges.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "boot_record.h"
#include "directory.h"
#include "example_disk.h"

namespace {

void PrintHexData(std::span<const std::byte> data) {
  const int bytes_per_line = 64;
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

  std::cout << mbr << std::endl;

  EXPECT_EQ(mbr.partitions[0].status, 0x80);
  EXPECT_EQ(
      mbr.partitions[0].chs_start,
      (ChsAddress{.head = 0, .sector = 1, .cylinder_hi = 0, .cylinder_lo = 0}));
  EXPECT_EQ(mbr.boot_signature, 0xAA'55);
}

TEST(ExampleDisk, Regions) {
  using Sector = std::byte[512];
  auto sectors = std::span(reinterpret_cast<const Sector*>(example_disk.data()),
                           example_disk.size() / sizeof(Sector));

  BootRecord mbr;
  std::memcpy(&mbr, sectors[0], sizeof(mbr));
  sectors = sectors.subspan(mbr.bpb.reserved_sectors);

  const auto fat_sectors =
      sectors.first(mbr.bpb.fats * mbr.bpb.sectors_per_fat);
  sectors = sectors.subspan(fat_sectors.size());

  const auto root_dir_sectors = sectors.first(mbr.root_dir_sectors());

  const auto data_sectors = sectors.subspan(root_dir_sectors.size());
  EXPECT_EQ(data_sectors.size(), mbr.data_sectors());

  PrintHexData(fat_sectors[0]);
  PrintHexData(root_dir_sectors[0]);

  const auto* entries = reinterpret_cast<const Directory::Entry*>(root_dir_sectors[0]);

  auto iter = Directory::Iterator(entries);
  while (iter != iter) {
    std::cout << std::string(*(iter++)) << std::endl;
  }
}

}  // namespace
