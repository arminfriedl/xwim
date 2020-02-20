#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <archive.h>
#include <fmt/format.h>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include "spec.hpp"

namespace xwim {

class Archive {
 private:
  std::filesystem::path path;
  archive* xwim_archive;

 public:
  explicit Archive(std::filesystem::path path);
  ~Archive();

  ArchiveSpec check();
  void extract(ExtractSpec extract_spec);
};

class ArchiveException : public std::exception {
 private:
  std::string _what;

 public:
  ArchiveException(std::string what, archive* archive) {
    if (archive_error_string(archive)) {
      _what = fmt::format("{}: {}", what, archive_error_string(archive));
    } else {
      _what = fmt::format("{}", what);
    }
  }

  virtual const char* what() const noexcept
  { return this->_what.c_str(); }
};

}  // namespace xwim

#endif
