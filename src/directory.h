#pragma once

#include <cstddef>

struct DirectoryEntry {
  std::uint8_t name[8];
  std::uint8_t extension[3];
  std::uint8_t attributes;
  std::uint8_t reserved;

  struct Time {
    std::uint8_t centiseconds;
    std::uint16_t biseconds : 5;
    std::uint16_t minutes : 6;
    std::uint16_t hours : 5;
  } __attribute__((packed));
  static_assert(sizeof(Time) == 3);

  struct Date {
    std::uint16_t day : 5;
    std::uint16_t month : 4;
    std::uint16_t year_offset_1980 : 7;
  } __attribute__((packed));
  static_assert(sizeof(Date) == 2);

  Time creation_time;
  Date creation_date;
  Date last_access_date;
  std::uint8_t extended_attributes;
  Time last_modified_time;
  Date last_modified_date;
  std::uint16_t start_cluster;
  std::uint32_t size;
} __attribute__((packed));
static_assert(sizeof(DirectoryEntry) == 32);
