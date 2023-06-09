#include "directory/raw.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string_view>

namespace fat::raw {

namespace {
std::string_view StripPadding(std::string_view str) {
  while (!str.empty() && str.back() == ' ') {
    str.remove_suffix(1);
  }
  return str;
}
}  // namespace

std::string Time::to_string() const {
  return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, 2 * biseconds);
}

std::string Date::to_string() const {
  return fmt::format("{:04d}/{:02d}/{:02d}", year_offset_1980 + 1980, month,
                     day);
}

std::string Entry::to_string() const {
  if (IsEnd()) {
    return "<END>";
  }
  if (IsDeleted()) {
    return "<DELETED>";
  }
  if (IsLfn()) {
    return fmt::format("lfn entry: {}", as_lfn());
  }
  if (IsVolume()) {
    return fmt::format("volume entry: {}", name());
  }
  return fmt::format(
      "8.3 entry: name={} attr={:#x} ctime={} cdate={} adate={} eattr={:#x} "
      "mtime={} "
      "mdate={} start_cluster={:#x} size={:#x}",
      name(), attributes, creation_time, creation_date, last_access_date,
      extended_attributes, last_modified_time, last_modified_date,
      start_cluster, size);
}

const LfnEntry& Entry::as_lfn() const {
  return reinterpret_cast<const LfnEntry&>(*this);
}

std::string LfnEntry::to_string() const {
  return fmt::format("seqnum={:x} is_last={} attr={:#x} (name)={}", seqnum,
                     is_last, attr, name());
}

std::u16string LfnEntry::name16() const {
  std::u16string str;
  auto append = [&](const auto& chars) {
    for (std::uint16_t c : chars) {
      if (c == 0 || c == 0xFF'FF) {
        break;
      }
      str.push_back(c);
    }
  };
  append(name0);
  append(name1);
  append(name2);
  return str;
}

std::string LfnEntry::name() const {
  std::string str;
  // Naive conversion that only handles ASCII.
  for (char16_t c16 : name16()) {
    if (c16 < 0xFF) {
      str.push_back(static_cast<char>(c16));
    } else {
      str.push_back('?');
    }
  }
  return str;
}

std::string Entry::name() const {
  const std::string_view stripped_extension = StripPadding(std::string_view(
      reinterpret_cast<const char*>(extension), sizeof(extension)));
  std::string out(StripPadding(
      std::string_view(reinterpret_cast<const char*>(stem), sizeof(stem))));
  if (!stripped_extension.empty()) {
    out += ".";
    out += stripped_extension;
  }
  return out;
}

}  // namespace fat::raw
