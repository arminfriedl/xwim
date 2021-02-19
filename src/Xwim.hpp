#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <exception>
#include <memory>
#include <set>
#include <stdexcept>

#include "Common.hpp"
#include "Archiver.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

enum class Action { UNKNOWN, EXTRACT, COMPRESS };

class Xwim {
 private:
  Action action;
  fs::path out;
  set<fs::path> ins;
  unique_ptr<Archiver> archiver;

  void infer_action();
  void infer_output();
  void infer_compression_output();
  void infer_extraction_output();

 public:
  Xwim();

  void try_infer();
  void dwim();

  void setCompress();
  void setExtract();
  void setOut(fs::path);
  void setIns(vector<fs::path> ins);
};

}  // namespace xwim

template <>
struct fmt::formatter<xwim::Action> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const xwim::Action& action, FormatContext& ctx) {
    switch (action) {
      case xwim::Action::UNKNOWN:
        return format_to(ctx.out(), "UNKNOWN");
      case xwim::Action::EXTRACT:
        return format_to(ctx.out(), "EXTRACT");
      case xwim::Action::COMPRESS:
        return format_to(ctx.out(), "COMPRESS");
    };
    return format_to(ctx.out(), "");
  }
};
