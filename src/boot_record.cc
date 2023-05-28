#include "boot_record.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <tuple>

bool ChsAddress::operator==(const ChsAddress& other) const {
  auto as_tuple = [](const ChsAddress& a) {
    return std::make_tuple(a.head, a.cylinder(), a.sector);
  };
  return as_tuple(*this) == as_tuple(other);
}

std::ostream& operator<<(std::ostream& s, const ChsAddress& chs) {
  return s << fmt::format("{{head={:#x} cylinder={:#x} sector={:#x}}}",
                          chs.head, chs.cylinder(), chs.sector);
}

std::ostream& operator<<(std::ostream& s, const PartitionEntry& e) {
  return s << fmt::format(
             "status={:#x} chs_start={} type={:#x} chs_end={} lba={:#x} "
             "sector_count={:#x}",
             e.status, fmt::streamed(e.chs_start), e.type,
             fmt::streamed(e.chs_end), e.lba, e.sector_count);
}
