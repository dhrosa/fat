#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>

class Directory {
 public:
  struct Entry;
  class Iterator;

  Directory(const Entry* entries) : entries_(entries) {}

  Iterator begin() const;
  Iterator end() const;

 private:
  const Entry* entries_;
};

struct Directory::Entry {
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
static_assert(sizeof(Directory::Entry) == 32);

template <typename T, typename... Choices>
concept IsAnyOf = (std::same_as<T, Choices> || ...);

template <typename T>
concept IsDirectoryType = IsAnyOf<T, Directory::Entry::Time,
                                  Directory::Entry::Date, Directory::Entry>;

// fmtlib extension point.
std::string format_as(IsDirectoryType auto const& t) { return t.to_string(); }

std::ostream& operator<<(std::ostream& s, IsDirectoryType auto const& t) {
  return s << t.to_string();
}

class Directory::Iterator {
 public:
  // Required to model std::forward_iterator
  using difference_type = std::ptrdiff_t;
  using value_type = const Directory::Entry;
  using iterator_category = std::forward_iterator_tag;
  Iterator() = default;

  Iterator(const Entry* entries) : entries_(entries) {}

  bool operator==(const Iterator&) const { return entry().IsEnd(); }

  const Entry& entry() const { return entries_[0]; }

  const Entry& operator*() const { return entry(); }
  const Entry& operator->() const { return entry(); }

  Iterator& operator++() {
    while (true) {
      ++entries_;
      if (entry().IsDeleted()) {
        continue;
      }
      return *this;
    }
  }

  Iterator operator++(int) {
    Iterator copy = *this;
    ++(*this);
    return copy;
  }

 private:
  const Entry* entries_ = nullptr;
};

inline Directory::Iterator Directory::begin() const {
  return Iterator(entries_);
}

inline Directory::Iterator Directory::end() const { return Iterator(nullptr); }
