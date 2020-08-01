#include <spdlog/common.h>
#include <cstdlib>
namespace logger = spdlog;

#include <iostream>
#include <ostream>
#include <string>
#include <list>

#include "util/log.hpp"
#include "util/argparse.hpp"
#include "archive.hpp"
#include "spec.hpp"
#include "fileformats.hpp"

int main(int argc, char** argv) {
  xwim::log::init();

  xwim::argparse::XwimPath xwim_path;

  try {
    xwim_path = xwim::argparse::parse(argc, argv);
  } catch (xwim::argparse::ArgParseException& ex) {
    logger::error("{}\n", ex.what());
    std::cout << xwim::argparse::usage();
    std::exit(1);
  }

  try {
    xwim::Archive archive{xwim_path.path()};
    xwim::ArchiveSpec archive_spec = archive.check();

    logger::info("{}", archive_spec);

    xwim::ExtractSpec extract_spec{};

    if (!archive_spec.has_single_root || !archive_spec.is_root_filename) {
      extract_spec.make_dir = true;

      std::filesystem::path stem = xwim::stem(xwim_path.path());

      extract_spec.dirname = stem;
    }

    if (archive_spec.has_subarchive) {
      extract_spec.extract_subarchive = true;
    }

    logger::info("{}", extract_spec);

    archive.extract(extract_spec);

  } catch (xwim::ArchiveException& ae) {
    logger::error("{}", ae.what());
  }
}
