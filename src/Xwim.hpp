#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <exception>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>

#include "Archiver.hpp"
#include "util/Common.hpp"
#include "UserOpt.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

enum class Action { EXTRACT, COMPRESS };

struct XwimIntent {
    
};

class XwimBuilder {
 private:
  UserOpt user_opt;

 public:
  XwimBuilder(UserOpt user_opt) : user_opt(user_opt){};
  Xwim build();
};

class Xwim {
 public:
  virtual XwimResult dwim() = 0;
};

class XwimCompressor : public Xwim {
private:
  fs::path archive;
  std::set<fs::path> paths;
};

class XwimExtractor : public Xwim {};

class XwimConfig {
 public:
  Action get_action();
}

class Xwim {
 private:
  XwimEngine xwim_engine;
  UserOpt user_opt;

 public:
  Xwim(UserOpt user_opt);
  void dwim();
}

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
  void sanitize_output();

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
