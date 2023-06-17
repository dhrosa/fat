add_custom_command(OUTPUT example.disk
  DEPENDS make_disk.py
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/make_disk.py ${CMAKE_CURRENT_BINARY_DIR}/example.disk
)

add_custom_command(OUTPUT example_disk_bin.o
  DEPENDS example.disk
  COMMAND ld -z noexecstack --relocatable --format=binary -o example_disk_bin.o example.disk
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(example_disk example_disk_bin.o)
set_target_properties(example_disk PROPERTIES LINKER_LANGUAGE C)

add_library(fat
  directory.cc
  boot_record.cc)
target_link_libraries(fat fmt range-v3)

include(GoogleTest)

add_executable(fat_test test.cc)
target_link_libraries(fat_test gmock gtest_main example_disk fat fmt)
gtest_discover_tests(fat_test)
