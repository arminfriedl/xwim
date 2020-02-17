#ifndef SPEC_H
#define SPEC_H

#include <archive.h>
#include <fmt/core.h>

#include <filesystem>
#include <memory>

namespace xwim {

struct ArchiveSpec {
  bool has_single_root = false;
  bool is_root_filename = false;
  bool is_root_dir = false;
  bool has_subarchive = false;
};

struct ExtractSpec {
  bool make_dir;
  std::filesystem::path dirname;
};

}  // namespace xwim

template <>
struct fmt::formatter<xwim::ArchiveSpec> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const xwim::ArchiveSpec& spec, FormatContext& ctx) {
    return format_to(ctx.out(),"Archive["
                     " .has_single_root={},"
                     " .is_root_filename={}"
                     " .is_root_dir={}"
                     " .has_subarchive={}"
                     " ]",
                     spec.has_single_root, spec.is_root_filename,
                     spec.is_root_dir, spec.has_subarchive);
  }
};

#endif
