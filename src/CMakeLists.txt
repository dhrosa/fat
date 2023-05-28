add_custom_command(OUTPUT example_disk_bin.o
  BYPRODUCTS example.disk
  COMMAND dd if=/dev/zero of=example.disk bs=1M count=16
  COMMAND mkfs.fat -v -F 16 --mbr=yes -n EXAMPLE --invariant example.disk
  COMMAND ld -z noexecstack --relocatable --format=binary -o example_disk_bin.o example.disk
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(example_disk example_disk_bin.o)
set_target_properties(example_disk PROPERTIES LINKER_LANGUAGE C)

add_library(fat
  boot_record.cc)
target_link_libraries(fat fmt)

include(GoogleTest)

add_executable(fat_test test.cc)
target_link_libraries(fat_test gmock gtest_main example_disk fat fmt)
gtest_discover_tests(fat_test)
