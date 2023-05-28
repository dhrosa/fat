#include "boot_record.h"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <span>
#include <string_view>
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

std::ostream& operator<<(std::ostream& s, const BiosParameterBlock& b) {
  return s << fmt::format(
             "trampoline={::#04x} oem_identifier={} bytes_per_sector={} "
             "sectors_per_cluster={} reserved_sectors={} fats={} "
             "root_entries={} sectors={:#x} media_descriptor_type={:#x} "
             "sectors_per_fat={} sectors_per_track={} heads={} "
             "hidden_sectors={} extended_sectors={}",
             b.trampoline, std::string_view(std::span(b.oem_identifier)),
             b.bytes_per_sector, b.sectors_per_cluster, b.reserved_sectors,
             b.fats, b.root_entries, b.sectors, b.media_descriptor_type,
             b.sectors_per_fat, b.sectors_per_track, b.heads, b.hidden_sectors,
             b.extended_sectors);
}
std::ostream& operator<<(std::ostream& s, const ExtendedBootRecord& b) {
  return s << fmt::format(
             "drive={} windows_nt_flags={:#x} signature={:#x} "
             "serial_number={:#x} label={} system_identifier={}",
             b.drive, b.windows_nt_flags, b.signature, b.serial_number,
             std::string_view(std::span(b.label)),
             std::string_view(std::span(b.system_identifier)));
}

std::ostream& operator<<(std::ostream& s, const PartitionEntry& e) {
  return s << fmt::format(
             "status={:#x} chs_start={} type={:#x} chs_end={} lba={:#x} "
             "sector_count={:#x}",
             e.status, fmt::streamed(e.chs_start), e.type,
             fmt::streamed(e.chs_end), e.lba, e.sector_count);
}

std::ostream& operator<<(std::ostream& s, const BootRecord& r) {
  s << "BPB: " << r.bpb;
  s << "\nEBPB: " << r.ebpb;
  for (int i = 0; i < std::size(r.partitions); ++i) {
    s << fmt::format("\npartitions[{}]: {}", i, fmt::streamed(r.partitions[i]));
  }
  s << fmt::format("\nboot_signature={:#06x}", r.boot_signature);
  return s;
}
