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

Directory::Entry::Time::operator std::string() const {
  return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, 2 * biseconds);
}

Directory::Entry::Date::operator std::string() const {
  return fmt::format("{:04d}/{:02d}/{:02d}", year_offset_1980 + 1980, month,
                     day);
}

Directory::Entry::operator std::string() const {
  if (IsEnd()) {
    return "<END>";
  }
  if (IsDeleted()) {
    return "<DELETED>";
  }
  if (attributes == 0x0F) {
    return "<LFN>";
  }
  if (attributes == 0x08) {
    return "<VOLUME LABEL>";
  }
  return fmt::format(
      "name={} attr={:#x} ctime={} cdate={} adate={} eattr={:#x} mtime={} "
      "mdate={} start_cluster={:#x} size={:#x}",
      name(), attributes, std::string(creation_time),
      std::string(creation_date), std::string(last_access_date),
      extended_attributes, std::string(last_modified_time),
      std::string(last_modified_date), start_cluster, size);
}

std::string Directory::Entry::name() const {
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