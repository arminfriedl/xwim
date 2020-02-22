#pragma once

#include <archive.h>

#include <filesystem>
#include <memory>

namespace xwim {

class ArchiveEntrySys {
 private:
  std::function<void (archive_entry*)> ae_deleter = [](archive_entry* ae) { archive_entry_free(ae); };
  std::unique_ptr<archive_entry, decltype(ae_deleter)> ae;
  friend class ArchiveExtractorSys;

 public:
  ArchiveEntrySys(std::unique_ptr<archive_entry> entry)
    : ae{ std::unique_ptr<archive_entry, decltype(ae_deleter)>{entry.release(), ae_deleter} }
  {}
  ArchiveEntrySys()
    : ae{ std::unique_ptr<archive_entry, decltype(ae_deleter)>{nullptr, ae_deleter} }
  {}
  ArchiveEntrySys(archive_entry* entry)
    : ae{ std::unique_ptr<archive_entry, decltype(ae_deleter)>{entry, ae_deleter} }
  {}

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
  ArchiveEntrySys cur_entry;
  friend class ArchiveExtractorSys;

 public:
  ArchiveReaderSys(std::filesystem::path& path);
  ~ArchiveReaderSys();

  /** Returns the next archive entry
   *
   * @return archive_entry or nullptr if end of archive reached
   */
  ArchiveEntrySys& next();

  /** Returns the current archive entry
   *
   * @return archive_entry or nullptr if current entry at end of archive
   */
  ArchiveEntrySys& cur();
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

  void extract(ArchiveReaderSys& reader, ArchiveEntrySys& entry);
  void extract_all(ArchiveReaderSys& reader);

  void extract_header(ArchiveEntrySys& entry);
  void extract_data();
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
  ArchiveSysException(std::string what) {
      _what = fmt::format("{}", what);
  }

  virtual const char* what() const noexcept { return this->_what.c_str(); }
};

}  // namespace xwim
