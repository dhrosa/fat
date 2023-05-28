#pragma once

#include <cstdint>
#include <ostream>

struct ChsAddress {
  std::uint8_t head;
  std::uint8_t sector : 6;
  std::uint8_t cylinder_hi : 2;
  std::uint8_t cylinder_lo;

  std::uint16_t cylinder() const {
    return static_cast<uint16_t>(cylinder_hi << 8) | cylinder_lo;
  }

  bool operator==(const ChsAddress& other) const;

} __attribute__((packed));
static_assert(sizeof(ChsAddress) == 3);

std::ostream& operator<<(std::ostream& s, const ChsAddress& chs);

struct PartitionEntry {
  std::uint8_t status;
  ChsAddress chs_start;
  std::uint8_t type;
  ChsAddress chs_end;
  std::uint32_t lba;
  std::uint32_t sector_count;
} __attribute__((packed));
static_assert(sizeof(PartitionEntry) == 16);

std::ostream& operator<<(std::ostream& s, const PartitionEntry& entry);

struct BootRecord {
  std::uint8_t bootstrap[446];
  PartitionEntry partitions[4];
  std::uint16_t boot_signature;
} __attribute__((packed));
static_assert(sizeof(BootRecord) == 512);
