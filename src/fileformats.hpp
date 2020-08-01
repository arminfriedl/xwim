/** @file fileformats.hpp
 * @brief Handle archive extensions
 */
#pragma once

#include <spdlog/spdlog.h>
#include <optional>
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
                                        ".rar", ".tar",   ".xz",   ".zip"};

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

/** Get the archive extension of a path.
 *
 * The archive extension may be a combination of supported fileformats in which
 * case all of them are returned.
 *
 * @returns Archive extension of the archive or path() if no (known) extension
 * exists.
 */
inline std::filesystem::path ext(const std::filesystem::path& path) {
  std::filesystem::path p_ext{path};
  logger::trace("Extracting extension of {}", p_ext.string());

  std::filesystem::path p_ext_collector;
  while (fileformats.find(p_ext.extension().string()) != fileformats.end()) {
    // path extension()  const
    p_ext_collector = p_ext.extension().concat(p_ext_collector.string());
    p_ext.replace_extension();
  }

  return p_ext_collector;
}

}  // namespace xwim
