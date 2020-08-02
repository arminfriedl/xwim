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

/** Archive filters
 *
 * Archive filters are essentially either data compression algorithms or data
 * encodings. Filters are used on archives after an archiving program created
 * the archive out of files and folders. Multiple filters can be applied to an
 * archive. The order is significant.
 *
 * The simplest way to understand the distinction between filters and formats is
 * to visualize the traditional `tar.gz` format. Tar creates the archive (an
 * archive format). Gzip compresses the archive (an archive filter). In theory
 * one could create a `tar.gz.lz.uu` tarball. That is, a `tar` archive
 * filter-compressed with `gzip`, filter-compressed with `lzip`, filter-encoded
 * with `uuencode`.
 *
 * Note that while this abstraction works in many cases it is not perfect. For
 * example `.zip` files are traditionally archives where every entry is
 * compressed separately and then bundled them together into an archive. In
 * those cases the archive format is ZIP with no (external) filters.
 */
enum archive_filter {
  NONE     = 0,
  GZIP     = 1,
  BZIP2    = 2,
  COMPRESS = 3,
  PROGRAM  = 4,
  LZMA     = 5,
  XZ       = 6,
  UU       = 7,
  RPM      = 8,
  LZIP     = 9,
  LRZIP    = 10,
  LZOP     = 11,
  GRZIP    = 12,
  LZ4      = 13,
  ZSTD     = 14
};

/** Archive formats
 *
 * Archive formats are the specifications for bundling together multiple files
 * and folders (including metadata) into a single file (the archive). See also
 * `archive_filter` for more details on the difference between archive formats
 * and archive filters.
 */
enum archive_format {
  BASE_MASK           = 0xff0000,
  CPIO                = 0x10000,
  CPIO_POSIX          = (CPIO | 1),
  CPIO_BIN_LE         = (CPIO | 2),
  CPIO_BIN_BE         = (CPIO | 3),
  CPIO_SVR4_NOCRC     = (CPIO | 4),
  CPIO_SVR4_CRC       = (CPIO | 5),
  CPIO_AFIO_LARGE     = (CPIO | 6),
  SHAR                = 0x20000,
  SHAR_BASE           = (SHAR | 1),
  SHAR_DUMP           = (SHAR | 2),
  TAR                 = 0x30000,
  TAR_USTAR           = (TAR | 1),
  TAR_PAX_INTERCHANGE = (TAR | 2),
  TAR_PAX_RESTRICTED  = (TAR | 3),
  TAR_GNUTAR          = (TAR | 4),
  ISO9660             = 0x40000,
  ISO9660_ROCKRIDGE   = (ISO9660 | 1),
  ZIP                 = 0x50000,
  EMPTY               = 0x60000,
  AR                  = 0x70000,
  AR_GNU              = (AR | 1),
  AR_BSD              = (AR | 2),
  MTREE               = 0x80000,
  RAW                 = 0x90000,
  XAR                 = 0xA0000,
  LHA                 = 0xB0000,
  CAB                 = 0xC0000,
  RAR                 = 0xD0000,
  SEVENZIP            = 0xE0000,
  WARC                = 0xF0000,
  RAR_V5              = 0x100000
};

/** Strip archive extensions from a path
 *
 * @returns Base filename without archive extensions
 */
inline std::filesystem::path
stem(const std::filesystem::path& path) {
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
