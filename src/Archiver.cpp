#include "Archiver.hpp"
#include "Formats.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <map>
#include <memory>

#include "util/Common.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix)
std::string default_extension = ".tar.gz";
#elif defined(_win32) || defined(__win32__) || defined(__windows__)
std::string default_extension = ".zip";
#else
    std::string default_extension = ".zip";
#endif

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

// Extract longest known extension from path
fs::path archive_extension(const fs::path& path) {
  // TODO: creates lots of paths, refactor
  fs::path ext;
  fs::path tmp_ext;
  fs::path tmp_path;

  // cater for trailing `/` which is represented
  // as empty path element
  for (auto p : path) {
    if (!p.empty()) {
      tmp_path /= p;
    }
  }

  while (tmp_path.has_extension()) {
    tmp_ext = tmp_path.extension() += tmp_ext;
    Format format = find_extension_format(tmp_ext);

    if (format != Format::UNKNOWN) {
      // (Combined) extension known. Remember as `ext` and keep
      // looking for even longer extensions.
      ext = tmp_ext;
    }  // else: (Combined) extension not known, keep `ext` as-is but try
       // longer extensions

    tmp_path = tmp_path.stem();
  }

  return ext;
}

// Strip longest known extension from path
fs::path strip_archive_extension(const fs::path& path) {
  // TODO: creates lots of paths, refactor
  int longest_ext = 0;
  int tmp_longest_ext = 0;
  fs::path tmp_ext;
  fs::path tmp_path;
  fs::path stem_path;

  // cater for trailing `/` which is represented
  // as empty path element
  for(auto p: path) {
    if(!p.empty()) {
      tmp_path /= p;
    }
  }
  stem_path = tmp_path;

  spdlog::debug("Checking {} extensions", tmp_path);

  while (tmp_path.has_extension()) {
    tmp_ext = tmp_path.extension() += tmp_ext;
    spdlog::debug("Looking for {} in known extensions", tmp_ext);

    Format format = find_extension_format(tmp_ext);
    tmp_longest_ext++;
    if (format != Format::UNKNOWN) {
      // (Combined) extension known. Remember as `longest_ext` and keep
      // looking for even longer extensions.
      longest_ext = tmp_longest_ext;
    }  // else: (Combined) extension not known, keep `longest_ext` as-is but try
       // longer extensions

    spdlog::debug("Stemming {} to {}", tmp_path, tmp_path.stem());
    tmp_path = tmp_path.stem();
  }

  spdlog::debug("Found {} extensions", longest_ext);
  tmp_path = stem_path;
  for (int i = 0; i < longest_ext; i++) tmp_path = tmp_path.stem();

  spdlog::debug("Stripped path is {} ", tmp_path);
  return tmp_path;
}

std::filesystem::path default_archive(const std::filesystem::path& base) {
    string base_s = base.string();
    string ext_s = default_extension;

    return fs::path{fmt::format("{}{}", base_s, ext_s)};
}

bool can_handle_archive(const fs::path& path) {
  fs::path ext = archive_extension(path);
  if (format_extensions.find(ext.string()) != format_extensions.end()) {
    spdlog::debug("Found {} in known formats", ext);
    return true;
  }

  spdlog::debug("Could not find {} in known formats", ext);
  return false;
}

Format parse_format(const fs::path& path) {
  spdlog::debug("Looking for path {}", path);
  fs::path ext = archive_extension(path);
  spdlog::debug("Looking for ext {}", ext);
  Format format = find_extension_format(ext);

  if (format == Format::UNKNOWN) {
    throw XwimError{"No known archiver for {}", path};
  }

  return format;
}

unique_ptr<Archiver> make_archiver(const string& archive_name) {
  switch (parse_format(archive_name)) {
      case Format::TAR_GZIP:      case Format::TAR_BZIP2:
      case Format::TAR_COMPRESS:  case Format::TAR_LZIP:
      case Format::TAR_XZ:        case Format::TAR_ZSTD:
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
