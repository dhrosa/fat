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

struct BiosParameterBlock {
  std::uint8_t trampoline[3];
  char oem_identifier[8];

  std::uint16_t bytes_per_sector;
  std::uint8_t sectors_per_cluster;
  std::uint16_t reserved_sectors;
  std::uint8_t fats;
  std::uint16_t root_entries;
  std::uint16_t sectors;
  std::uint8_t media_descriptor_type;
  std::uint16_t sectors_per_fat;
  std::uint16_t sectors_per_track;
  std::uint16_t heads;
  std::uint32_t hidden_sectors;
  std::uint32_t extended_sectors;
} __attribute__((packed));

std::ostream& operator<<(std::ostream& s, const BiosParameterBlock& b);

struct ExtendedBootRecord {
  std::uint8_t drive;
  std::uint8_t windows_nt_flags;
  std::uint8_t signature;
  std::uint32_t serial_number;
  char label[11];
  char system_identifier[8];
} __attribute__((packed));

std::ostream& operator<<(std::ostream& s, const ExtendedBootRecord& b);

struct BootRecord {
  BiosParameterBlock bpb;
  ExtendedBootRecord ebpb;
  std::uint8_t unused[384];
  PartitionEntry partitions[4];
  std::uint16_t boot_signature;
} __attribute__((packed));
static_assert(sizeof(BootRecord) == 512);

std::ostream& operator<<(std::ostream& s, const BootRecord& r);
