#include "directory.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string_view>

namespace {
std::string_view StripPadding(std::string_view str) {
  while (!str.empty() && str.back() == ' ') {
    str.remove_suffix(1);
  }
  return str;
}
}  // namespace

std::string Directory::RawEntry::Time::to_string() const {
  return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, 2 * biseconds);
}

std::string Directory::RawEntry::Date::to_string() const {
  return fmt::format("{:04d}/{:02d}/{:02d}", year_offset_1980 + 1980, month,
                     day);
}

std::string Directory::RawEntry::to_string() const {
  if (IsEnd()) {
    return "<END>";
  }
  if (IsDeleted()) {
    return "<DELETED>";
  }
  if (attributes == 0x0F) {
    return "LFN: " + reinterpret_cast<const RawLfnEntry&>(*this).to_string();
    // return "<LFN>";
  }
  if (attributes == 0x08) {
    return "<VOLUME LABEL>";
  }
  return fmt::format(
      "name={} attr={:#x} ctime={} cdate={} adate={} eattr={:#x} mtime={} "
      "mdate={} start_cluster={:#x} size={:#x}",
      name(), attributes, creation_time, creation_date, last_access_date,
      extended_attributes, last_modified_time, last_modified_date,
      start_cluster, size);
}

std::string Directory::RawLfnEntry::to_string() const {
  return fmt::format("seqnum={:x} is_last={} attr={:#x} (name)={}", seqnum,
                     is_last, attr, name());
}

std::u16string Directory::RawLfnEntry::name16() const {
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

std::string Directory::RawLfnEntry::name() const {
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

std::string Directory::RawEntry::name() const {
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
