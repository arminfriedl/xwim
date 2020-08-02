#pragma once

#include <archive.h>

#include <filesystem>
#include <memory>
#include "spec.hpp"
#include <fmt/format.h>

namespace xwim {

/** A view into an archive entry
 *
 * The view is non-owning and the caller must guarantee
 * that the parent archive entry is valid when the view
 * is accessed.
 */
class ArchiveEntryView {
 private:
  archive_entry* ae;

 public:
  ArchiveEntryView() = default;
  ArchiveEntryView(archive_entry* entry) : ae{entry} {}

  bool is_empty();
  std::string path_name();
  std::filesystem::path path();
  mode_t file_type();
  bool is_directory();
};

/** A reader for archive files
 *
 * Shim for `libarchive`. Iterates through
 * entries of an archive with `next()`
 */
class ArchiveReaderSys {
 private:
  archive* ar;
  archive_entry* ae;
  friend class ArchiveExtractorSys;

 public:
  ArchiveReaderSys(std::filesystem::path& path);
  ~ArchiveReaderSys();

  /** Advances the internal entry pointer
   *
   * @return true if the pointer advanced to the next entry
   *         false if the end of the archive was reached
   */
  bool advance();

  /** Returns a non-owning view of the current entry
   *
   * ArchiveEntryView is a non-owning view of the currently
   * active entry in this reader. A retrieved archive entry
   * may not be used after another call to advance in the
   * same reader.
   *
   * @return a view to the archive entry this reader currently
   *         points to
   */
  const ArchiveEntryView cur();
};

/** A extractor for archive files
 *
 * Shim for `libarchive`.
 */
class ArchiveExtractorSys {
 private:
  archive* writer;

 public:
  ArchiveExtractorSys(std::filesystem::path& root);
  ArchiveExtractorSys();
  ~ArchiveExtractorSys();

  void extract_all(ArchiveReaderSys& reader);
  void extract_entry(ArchiveReaderSys& reader);
};

/** A compressor for archive files
 *
 * Shim for `libarchive`
 */
class ArchiveCompressorSys {
private:
  archive* new_archive;
  std::filesystem::path root;
  xwim::CompressSpec compress_spec;

public:
  ArchiveCompressorSys(std::filesystem::path& root, xwim::CompressSpec compress_spec);
  ~ArchiveCompressorSys();

  void compress();
};

class ArchiveSysException : public std::exception {
 private:
  std::string _what;

 public:
  ArchiveSysException(std::string what, archive* archive) {
    if (archive_error_string(archive)) {
      _what = fmt::format("{}: {}", what, archive_error_string(archive));
    } else {
      _what = fmt::format("{}", what);
    }
  }
  ArchiveSysException(std::string what) { _what = fmt::format("{}", what); }

  virtual const char* what() const noexcept { return this->_what.c_str(); }
};

}  // namespace xwim
