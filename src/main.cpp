#include <spdlog/common.h>
#include <cstdlib>

#include <iostream>
#include <ostream>
#include <string>
#include <list>

#include "util/log.hpp"
#include "util/argparse.hpp"
#include "archive.hpp"
#include "spec.hpp"
#include "fileformats.hpp"

namespace logger = spdlog;
using namespace xwim::argparse;

void extract(const XwimPath& xwim_path) {
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

void compress(const XwimPath& xwim_path) {
  try {
    xwim::Archive archive{xwim_path.path()};
    xwim::CompressSpec compress_spec{};

    archive.compress(compress_spec);

  } catch (xwim::ArchiveException& ae) {
    logger::error("{}", ae.what());
  }
}

XwimPath parse_args(int argc, char** argv) {
  try {
    return parse(argc, argv);
  } catch (ArgParseException& ex) {
    logger::error("{}\n", ex.what());
    std::cout << usage();
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  xwim::log::init();

  XwimPath xwim_path = parse_args(argc, argv);

  if(xwim_path.is_archive()) { extract(xwim_path); }
  else { compress(xwim_path); }
}
