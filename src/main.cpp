#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <memory>

#include "Common.hpp"
#include "Opt.hpp"
#include "Log.hpp"
#include "Xwim.hpp"

using namespace xwim;
using namespace std;

int main(int argc, char** argv) {
  log::init();

  Opt opt;
  opt.parse(argc, argv);
  opt.validate();

  Xwim xwim{};

  try {
    xwim.dwim();
  } catch (XwimError& e) {
    spdlog::error(e.what());
  }
}
