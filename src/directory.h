#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>

class Directory {
 public:
  struct RawEntry;
  class RawIterator;

  Directory(const RawEntry* entries) : entries_(entries) {}

  RawIterator begin() const;
  RawIterator end() const;

 private:
  const RawEntry* entries_;
};

struct Directory::RawEntry {
  std::uint8_t stem[8];
  std::uint8_t extension[3];
  std::uint8_t attributes;
  std::uint8_t reserved;

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

} __attribute__((packed));
static_assert(sizeof(Directory::RawEntry) == 32);

template <typename T, typename... Choices>
concept IsAnyOf = (std::same_as<T, Choices> || ...);

template <typename T>
concept IsDirectoryType =
    IsAnyOf<T, Directory::RawEntry::Time, Directory::RawEntry::Date,
            Directory::RawEntry>;

// fmtlib extension point.
std::string format_as(IsDirectoryType auto const& t) { return t.to_string(); }

std::ostream& operator<<(std::ostream& s, IsDirectoryType auto const& t) {
  return s << t.to_string();
}

class Directory::RawIterator {
 public:
  // Required to model std::forward_iterator
  using difference_type = std::ptrdiff_t;
  using value_type = const Directory::RawEntry;
  using iterator_category = std::forward_iterator_tag;
  RawIterator() = default;

  RawIterator(const RawEntry* entries) : entries_(entries) {}

  bool operator==(const RawIterator&) const { return entry().IsEnd(); }

  const RawEntry& entry() const { return entries_[0]; }

  const RawEntry& operator*() const { return entry(); }
  const RawEntry& operator->() const { return entry(); }

  RawIterator& operator++() {
    while (true) {
      ++entries_;
      if (entry().IsDeleted()) {
        continue;
      }
      return *this;
    }
  }

  RawIterator operator++(int) {
    RawIterator copy = *this;
    ++(*this);
    return copy;
  }

 private:
  const RawEntry* entries_ = nullptr;
};

inline Directory::RawIterator Directory::begin() const {
  return RawIterator(entries_);
}

inline Directory::RawIterator Directory::end() const {
  return RawIterator(nullptr);
}
