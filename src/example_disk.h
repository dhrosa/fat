#pragma once
#include <span>

extern std::byte _binary_example_disk_start[];
extern std::byte _binary_example_disk_end[];

const std::span<const std::byte> example_disk(_binary_example_disk_start, _binary_example_disk_end);
