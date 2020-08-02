#pragma once

#include <archive.h>
#include <fmt/format.h>

#include <filesystem>
#include <memory>

#include "fileformats.hpp"

namespace xwim {

/** Properties of an archive
 *
 * These properties can be retrieved by analyzing the
 * archive. There is no outside-knowledge. All information
 * is in the archive.
 */
struct ArchiveSpec {
  bool has_single_root = false;  /** There is only a single file xor a single
                                    folder at the archive's root */
  bool is_root_filename = false; /** the name of the (single) root is the same
                                    as the stemmed archive file name. Cannot be
                                    true if `has_single_root` is false */
  bool is_root_dir = false; /** The (single) root is a folder. Cannot be true if
                               `has_single_root` is false */
  bool has_subarchive = false; /** Whether the archive contains sub-archives */
};

/** Properties influencing the extraction process
 *
 * These properties can be set to influence the extraction
 * process accordingly.
 */
struct ExtractSpec {
  bool make_dir = false; /** Create a new directory for extraction at `dirname` */
  std::filesystem::path dirname{}; /** The path to a directory for extraction */
  bool extract_subarchive = false; /** Recursively extract sub-archives */
};


/** Compile time definitions for platform-dependent files and filters */
#if defined(unix) || defined(__unix__) || defined(__unix)
#define XWIM_COMPRESS_FORMAT xwim::archive_format::TAR_USTAR
#define XWIM_COMPRESS_FILTER { xwim::archive_filter::GZIP }
#define XWIM_COMPRESS_EXTENSION ".tar.gz"
#elif defined(_win32) || defined(__win32__) || defined(__windows__)
#define XWIM_COMPRESS_FORMAT xwim::archive_format::ZIP
#define XWIM_COMPRESS_FILTER {}
#define XWIM_COMPRESS_EXTENSION ".zip"
#else
#define XWIM_COMPRESS_FORMAT xwim::fileformats::archive_format::ZIP
#define XWIM_COMPRESS_FILTER {}
#define XWIM_COMPRESS_EXTENSION ".zip"
#endif

/** Properties influencing the compression process
 *
 * These properties can be set to influence the compression process
 * accordingly.
 *
 * Per default, the compress spec is platform dependent to accommodate for the
 * expected format on that platform. On Windows this is zip, on Unix this is
 * tar.gz
 */
struct CompressSpec {
  xwim::archive_format format =
      XWIM_COMPRESS_FORMAT; /** The archiving format, e.g. tar */
  std::vector<xwim::archive_filter> filters =
      XWIM_COMPRESS_FILTER; /** Filters applied to the archive,
                                e.g. gzip */
  std::string extension =
    XWIM_COMPRESS_EXTENSION; /** Archive extension, e.g. .tar.gz */
};

}  // namespace xwim

#if FMT_VERSION < 50300
typedef fmt::basic_parse_context<char> format_parse_context;
#endif

template <>
struct fmt::formatter<xwim::ArchiveSpec> {
  constexpr auto parse(format_parse_context & ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const xwim::ArchiveSpec& spec, FormatContext& ctx) {
    return format_to(ctx.out(),
                     "Archive["
                     " .has_single_root={},"
                     " .is_root_filename={}"
                     " .is_root_dir={}"
                     " .has_subarchive={}"
                     " ]",
                     spec.has_single_root, spec.is_root_filename,
                     spec.is_root_dir, spec.has_subarchive);
  }
};

template <>
struct fmt::formatter<xwim::ExtractSpec> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const xwim::ExtractSpec& spec, FormatContext& ctx) {
    return format_to(ctx.out(),
                     "Extract["
                     " .make_dir={},"
                     " .dirname={}"
                     " .extract_subarchive={}"
                     " ]",
                     spec.make_dir, spec.dirname.string(),
                     spec.extract_subarchive);
  }
};

template <>
struct fmt::formatter<xwim::CompressSpec> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const xwim::CompressSpec& spec, FormatContext& ctx) {
    return format_to(ctx.out(),
                     "Compress["
                     " .format={},"
                     " .filters={}"
                     " ]",
                     spec.format, spec.filters);
  }
};
