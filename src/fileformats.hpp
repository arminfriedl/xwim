/** @file fileformats.hpp
 * @brief Handle archive extensions
 */
#pragma once

#include <spdlog/spdlog.h>
namespace logger = spdlog;

#include <filesystem>
#include <set>
#include <string>

namespace xwim {

/** Common archive formats understood by xwim
 *
 * The underlying libarchive backend retrieves format information by a process
 * called `bidding`. Hence, this information is mainly used to strip extensions.
 *
 * Stripping extensions via `std::filesystem::path` does not work reliably since
 * it gets easily confused by dots in the regular file name.
 */
const std::set<std::string> fileformats{".7z",  ".7zip",  ".jar", ".tgz",
                                        ".bz2", ".bzip2", ".gz",  ".gzip",
                                        ".rar", ".tar",   "xz",   ".zip"};

/** Strip archive extensions from a path
 *
 * @returns Base filename without archive extensions
 */
inline std::filesystem::path stem(const std::filesystem::path& path) {
  std::filesystem::path p_stem{path};
  logger::trace("Stemming {}", p_stem.string());

  p_stem = p_stem.filename();

  while (fileformats.find(p_stem.extension().string()) != fileformats.end()) {
    p_stem = p_stem.stem();
    logger::trace("Stemmed to {}", p_stem.string());
  }

  logger::trace("Finished stemming {}", p_stem.string());
  return p_stem;
}

}  // namespace xwim
