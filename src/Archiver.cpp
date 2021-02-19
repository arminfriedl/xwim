#include "Archiver.hpp"

#include <spdlog/spdlog.h>

#include <map>
#include <memory>

#include "Common.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

// Extract longest known extension from path
fs::path archive_extension(const fs::path& path) {
  // TODO: creates lots of paths, refactor
  fs::path ext;
  fs::path tmp_path = path;
  while (tmp_path.has_extension()) {
    fs::path tmp_ext = tmp_path.extension() += ext;
    auto search = extensions_format.find(tmp_ext);

    // (Combined) extension not known, return last known extension
    if (search == extensions_format.end()) return ext;

    // Continue extending extension
    ext = tmp_ext;
    tmp_path = tmp_path.stem();
  }

  return ext;
}

// Strip longest known extension from path
fs::path strip_archive_extension(const fs::path& path) {
  // TODO: creates lots of paths, refactor
  fs::path ext;
  fs::path tmp_path = path;
  while (tmp_path.has_extension()) {
    fs::path tmp_ext = tmp_path.extension() += ext;
    auto search = extensions_format.find(tmp_ext);

    // (Combined) extension not known, return stripped path
    if (search == extensions_format.end()) return tmp_path;

    // Continue stripping path
    ext = tmp_ext;
    tmp_path = tmp_path.stem();
  }

  return tmp_path;
}

bool can_extract(const fs::path& path) {
  fs::path ext = archive_extension(path);
  if (format_extensions.find(ext.string()) != format_extensions.end()) {
    spdlog::debug("Found {} in known formats", ext);
    return true;
  }

  spdlog::debug("Could not find {} in known formats", ext);
  return false;
}

Format parse_format(const fs::path& path) {
  fs::path ext = archive_extension(path);
  auto search = extensions_format.find(ext);
  if (search == extensions_format.end()) {
    throw XwimError{"No known archiver for {}", path};
  }

  return search->second;
}

unique_ptr<Archiver> make_archiver(const string& archive_name) {
  switch (parse_format(archive_name)) {
    case Format::TAR_GZ:
    case Format::ZIP:
      return make_unique<LibArchiver>();
    default:
      throw XwimError{
          "Cannot construct archiver for {}. `extension_format` surjection "
          "invariant violated?",
          archive_name};
  };
}
}  // namespace xwim
