#pragma once

#include <archive.h>
#include <fmt/format.h>

#include <filesystem>
#include <memory>

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
  bool is_root_dir = false; /** The (single) root is a folder. Cnnot be true if
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
