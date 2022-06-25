#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <optional>

#include "UserOpt.hpp"
#include "UserIntent.hpp"
#include "util/Common.hpp"
#include "util/Log.hpp"

using namespace xwim;
using namespace std;

int main(int argc, char** argv) {
  log::init();

  UserOpt user_opt = UserOpt{argc, argv};
  try {
      unique_ptr<UserIntent> user_intent = make_intent(user_opt);
      user_intent->execute();
  } catch(XwimError& e) {
      spdlog::error(e.what());
  }
}