#include <spdlog/common.h>
#include <spdlog/spdlog.h>
namespace logger = spdlog;

#include <iostream>
#include <ostream>
#include <string>

#include "archive.hpp"
#include "spec.hpp"

int main(int argc, char** argv) {
  logger::set_level(logger::level::trace);
  logger::flush_on(logger::level::trace);

  try {
    std::string filename{argv[1]};
    xwim::Archive archive{filename};
    xwim::ArchiveSpec spec = archive.check();

    logger::info("{}", spec);

  } catch (xwim::ArchiveException& ae) {
    logger::error("{}", ae.what());
  }
}
