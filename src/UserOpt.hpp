#pragma once

#include <optional>
#include <set>

#include "util/Common.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

struct UserOpt {
  bool compress;
  bool extract;
  bool interactive;
  std::optional<fs::path> out;
  std::set<fs::path> paths;

  UserOpt(int argc, char** argv);
};

}  // namespace xwim
