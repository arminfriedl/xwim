#ifndef SPEC_H
#define SPEC_H

#include <filesystem>
namespace xwim {
  struct ArchiveSpec {
    bool is_root_dir;
    bool is_root_dir_filename;
    bool has_subarchive;
  };

  struct ExtractSpec {
    bool make_dir;
    std::filesystem::path dirname;
  };
}

#endif
