#pragma once

#include <fmt/core.h>
#include <filesystem>
#include <string>
#include <random>

template <>
struct fmt::formatter<std::filesystem::path> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const std::filesystem::path& path, FormatContext& ctx) {
    return format_to(ctx.out(), path.string());
  }
};

class XwimError : public std::runtime_error {
 public:
  template <typename... Args>
  XwimError(const std::string& fmt, const Args... args)
      : std::runtime_error(fmt::format(fmt, args...)){}
};

inline int rand_int(int from, int to) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(from, to);
  return distrib(gen);
}
