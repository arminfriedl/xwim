#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <filesystem>
#include <optional>
#include <set>

namespace xwim {

enum class Action { EXTRACT, COMPRESS };

struct UserOpt {
  std::optional<Action> action;
  bool interactive;
  std::optional<std::filesystem::path> out;
  std::set<std::filesystem::path> paths;
};

struct XwimConfig {
  Action action;
  std::filesystem::path output;
  std::set<std::filesystem::path> input;
};

XwimConfig guess_wim(UserOpt user_opt);
void do_wim(XwimConfig);

}  // namespace xwim

template <>
struct fmt::formatter<xwim::Action> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const xwim::Action& action, FormatContext& ctx) {
    switch (action) {
      case xwim::Action::EXTRACT:
        return format_to(ctx.out(), "EXTRACT");
      case xwim::Action::COMPRESS:
        return format_to(ctx.out(), "COMPRESS");
    };
    return format_to(ctx.out(), "");
  }
};
