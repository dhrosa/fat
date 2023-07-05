#include "boot_record.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <span>
#include <string_view>
#include <tuple>

namespace fat {

bool ChsAddress::operator==(const ChsAddress& other) const {
  auto as_tuple = [](const ChsAddress& a) {
    return std::make_tuple(a.head, a.cylinder(), a.sector);
  };
  return as_tuple(*this) == as_tuple(other);
}

std::string ChsAddress::to_string() const {
  return fmt::format("{{head={:#x} cylinder={:#x} sector={:#x}}}", head,
                     cylinder(), sector);
}

std::string BiosParameterBlock::to_string() const {
  return fmt::format(
      "trampoline={::#04x} oem_identifier={} bytes_per_sector={} "
      "sectors_per_cluster={} reserved_sectors={} fats={} "
      "root_entries={} sectors={:#x} media_descriptor_type={:#x} "
      "sectors_per_fat={} sectors_per_track={} heads={} "
      "hidden_sectors={} extended_sectors={}",
      trampoline, std::string_view(std::span(oem_identifier)), bytes_per_sector,
      sectors_per_cluster, reserved_sectors, fats, root_entries, sectors,
      media_descriptor_type, sectors_per_fat, sectors_per_track, heads,
      hidden_sectors, extended_sectors);
}

std::string ExtendedBootRecord::to_string() const {
  return fmt::format(
      "drive={} windows_nt_flags={:#x} signature={:#x} "
      "serial_number={:#x} label={} system_identifier={}",
      drive, windows_nt_flags, signature, serial_number,
      std::string_view(std::span(label)),
      std::string_view(std::span(system_identifier)));
}

std::string PartitionEntry::to_string() const {
  return fmt::format(
      "status={:#x} chs_start={} type={:#x} chs_end={} lba={:#x} "
      "sector_count={:#x}",
      status, chs_start, type, chs_end, lba, sector_count);
}

std::string BootRecord::to_string() const {
  std::string partitions_str;
  for (int i = 0; i < std::size(partitions); ++i) {
    const PartitionEntry& partition = partitions[i];
    if (partition.status == 0) {
      continue;
    }
    fmt::format_to(std::back_inserter(partitions_str), "\npartitions[{}]: {}",
                   i, partition);
  }

  return fmt::format(
      "bpb: {}\n"
      "ebpb: {}\n"
      "boot_signature: {:#06x}\n"
      "(total_sectors)={:#x}\n"
      "(root_dir_sectors)={:#x}\n"
      "(first_data_sector)={:#x}\n"
      "(data_sectors)={:#x}\n"
      "(clusters)={:#x}"
      "{}",
      bpb, ebpb, boot_signature, total_sectors(), root_dir_sectors(),
      first_data_sector(), data_sectors(), clusters(), partitions_str);
  return partitions_str;
}

}  // namespace fat
