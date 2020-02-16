#include "archive.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "spec.hpp"

namespace xwim {
Archive::Archive(std::string path) : path{std::filesystem::path(path)} {}
Archive::Archive(std::filesystem::path&& path) : path{path} {}

ArchiveSpec Archive::check() {
  return ArchiveSpec{.is_root_dir = true,
                     .is_root_dir_filename = true,
                     .has_subarchive = false};
}

void Archive::extract(ExtractSpec extract_spec) {
  std::cout << "Make dir:" << extract_spec.make_dir
            << "Dirname: " << extract_spec.dirname
            << "Archname: " << this->path
            << std::endl;
}

}  // namespace xwim
