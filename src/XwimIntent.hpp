#pragma once

#include <optional>
#include <set>

#include "util/Common.hpp"

namespace xwim {
using namespace std;
using std::filesystem::path;

enum class Action { COMPRESS, EXTRACT };
struct UserIntent {
  Action action;
  path out;
  set<path> paths;
};

class UserOpt {
private:
  bool compress = true;
  bool extract = false;
  bool interactive = true;
  optional<path> out = nullopt;
  vector<path> paths = std::vector<path>{};

  Action action_intent();
  path out_intent();
  set<path> paths_intent();

 public:
  void parse_config(path config);
  void parse_args(int argc, char** argv);

  UserIntent guess_intent();
};

}  // namespace xwim
