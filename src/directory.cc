#include "directory.h"

#include <fmt/ranges.h>

#include <stdexcept>

namespace fat {

std::string Entry::to_string() const {
  return fmt::format("(long_name)={} sfn={}", long_name_, sfn_.to_string());
}

}  // namespace fat
