#pragma once

#include <fmt/core.h>

namespace fat {

template <typename T>
inline constexpr bool is_printable = false;

template <typename T>
concept Printable = is_printable<T>;

}  // namespace fat

template <fat::Printable T>
struct fmt::formatter<T> : fmt::formatter<fmt::string_view> {
  auto format(const T& x, fmt::format_context& context) {
    return formatter<fmt::string_view>::format(x.to_string(), context);
  }
};

std::ostream& operator<<(std::ostream& s, fat::Printable auto const& x) {
  return s << x.to_string();
}
