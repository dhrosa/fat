cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)

project(fat LANGUAGES C CXX)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0
)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.0.0)

FetchContent_MakeAvailable(googletest fmt)

add_subdirectory(src)
