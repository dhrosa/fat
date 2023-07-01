#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <range/v3/all.hpp>
#include <span>
#include <string>

#include "directory/raw.h"

namespace fat {

class Entry {
 public:
  std::string to_string() const;

 private:
  friend class Directory;

  Entry(raw::Entry sfn, std::string long_name)
      : sfn_(sfn), long_name_(long_name) {}

  raw::Entry sfn_;
  std::string long_name_;
};

class Directory {
 public:
  Directory(const raw::Entry* entries) : entries_(entries) {}

  auto raw() const;

  ranges::forward_range auto entries() const;

 private:
  const raw::Entry* entries_;
};

inline auto Directory::raw() const {
  struct PointerRange : public ranges::view_facade<PointerRange> {
    friend ranges::range_access;
    const raw::Entry* entries;
    bool equal(ranges::default_sentinel_t) const { return read().IsEnd(); }
    bool equal(const PointerRange& other) const {
      return entries == other.entries;
    }
    const raw::Entry& read() const { return entries[0]; }
    void next() { ++entries; }
  };
  return PointerRange{.entries = entries_};
}

inline ranges::forward_range auto Directory::entries() const {
  namespace views = ranges::views;
  return ranges::subrange(entries_, std::unreachable_sentinel_t{}) |
         views::take_while([](const raw::Entry& e) { return not e.IsEnd(); }) |
         views::filter([](const raw::Entry& entry) {
           return not(entry.IsVolume() or entry.IsDeleted());
         }) |
         views::chunk_by([](const raw::Entry& a, const raw::Entry& b) {
           // Continue chunk as long as left side is an LFN entry.
           return a.IsLfn();
         }) |
         views::transform([](const auto& chunk) {
           auto entries = chunk | views::reverse;
           const raw::Entry& sfn = entries.front();
           std::string long_name;
           for (const raw::Entry& entry : entries | views::drop(1)) {
             long_name += entry.as_lfn().name();
           }
           return Entry(sfn, long_name);
         });
}

}  // namespace fat
