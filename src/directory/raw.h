#pragma once

#include <cstdint>
#include <string>

namespace fat::raw {

struct Time {
  std::uint8_t centiseconds;
  std::uint16_t biseconds : 5;
  std::uint16_t minutes : 6;
  std::uint16_t hours : 5;

  std::string to_string() const;

} __attribute__((packed));
static_assert(sizeof(Time) == 3);

struct Date {
  std::uint16_t day : 5;
  std::uint16_t month : 4;
  std::uint16_t year_offset_1980 : 7;

  std::string to_string() const;
} __attribute__((packed));
static_assert(sizeof(Date) == 2);

struct LfnEntry;

struct Entry {
  std::uint8_t stem[8];
  std::uint8_t extension[3];
  std::uint8_t attributes;
  std::uint8_t reserved;

  Time creation_time;
  Date creation_date;
  Date last_access_date;
  std::uint8_t extended_attributes;
  Time last_modified_time;
  Date last_modified_date;
  std::uint16_t start_cluster;
  std::uint32_t size;

  std::string to_string() const;

  std::string name() const;

  bool IsEnd() const { return stem[0] == 0; }

  bool IsDeleted() const { return stem[0] == 0xE5; }

  bool IsLfn() const { return attributes == 0x0F; }

  bool IsVolume() const { return attributes == 0x08; }

  const LfnEntry& as_lfn() const;

} __attribute__((packed));
static_assert(sizeof(Entry) == 32);

struct LfnEntry {
  std::uint8_t seqnum : 5;
  std::uint8_t unused0 : 1;
  bool is_last : 1;
  std::uint8_t unused1 : 1;

  std::uint16_t name0[5];
  std::uint8_t attr;
  std::uint8_t type;
  std::uint8_t checksum;
  std::uint16_t name1[6];
  std::uint16_t start_cluster;
  std::uint16_t name2[2];

  std::string to_string() const;

  std::u16string name16() const;

  std::string name() const;

} __attribute__((packed));
static_assert(sizeof(LfnEntry) == sizeof(Entry));

template <typename T, typename... Choices>
concept IsAnyOf = (std::same_as<T, Choices> || ...);

template <typename T>
concept IsDirectoryType = IsAnyOf<T, Time, Date, Entry, LfnEntry>;

// fmtlib extension point.
std::string format_as(IsDirectoryType auto const& t) { return t.to_string(); }

std::ostream& operator<<(std::ostream& s, IsDirectoryType auto const& t) {
  return s << t.to_string();
}

}  // namespace fat::raw
