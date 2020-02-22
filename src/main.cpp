#include <spdlog/common.h>
#include <spdlog/spdlog.h>
namespace logger = spdlog;

#include <iostream>
#include <ostream>
#include <string>
#include <list>

#include "archive.hpp"
#include "spec.hpp"

int main(int argc, char** argv) {
  logger::set_level(logger::level::trace);

  try {
    std::filesystem::path filepath{argv[1]};
    xwim::Archive archive{filepath};
    xwim::ArchiveSpec archive_spec = archive.check();

    logger::info("{}", archive_spec);

    xwim::ExtractSpec extract_spec{};

    if (!archive_spec.has_single_root || !archive_spec.is_root_filename) {
      extract_spec.make_dir = true;

      std::filesystem::path stem = filepath.stem();

      while (stem.has_extension())
        stem = stem.stem();

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
