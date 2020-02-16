#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <filesystem>
#include <string>

#include "spec.hpp"

namespace xwim {

class Archive {
 private:
  std::filesystem::path path;

 public:
  explicit Archive(std::string path);
  explicit Archive(std::filesystem::path&& path);

  ArchiveSpec check();
  void extract(ExtractSpec normalize_spec);
};

}  // namespace xwim

#endif
