#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <optional>

#include "UserOpt.hpp"
#include "util/Common.hpp"
#include "util/Log.hpp"

using namespace xwim;
using namespace std;

int main(int argc, char** argv) {
  log::init();

  UserOpt user_opt = UserOpt{argc, argv};
}
