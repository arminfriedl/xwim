#pragma once

#include <optional>
#include <set>

#include "util/Common.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

struct UserOpt {
  optional<bool> compress;
  optional<bool> extract;
  bool interactive;
  int verbosity;
  std::optional<fs::path> out;
  std::set<fs::path> paths;

  UserOpt(int argc, char** argv);

  bool wants_compress() const {
      return this->compress.has_value() && this->compress.value();
  }

  bool wants_extract() const {
      return this->extract.has_value() && this->extract.value();
  }
};

}  // namespace xwim
