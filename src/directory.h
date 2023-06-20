#pragma once

#include <cstddef>
#include <iostream>
#include <iterator>
#include <range/v3/all.hpp>
#include <span>
#include <string>

#include "directory/raw.h"

namespace fat {

class Directory {
 public:
  Directory(const raw::Entry* entries) : entries_(entries) {}

  auto raw() const;

  auto raw_groups() const;

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

inline auto Directory::raw_groups() const {
  return raw() |
         ranges::views::chunk_by([](const raw::Entry& a, const raw::Entry& b) {
           // Element A and B belongs in the same chunk as long as A is an LFN
           // entry.
           return a.IsLfn();
         });
}

}  // namespace fat
