#include <archive_entry.h>
#include <spdlog/spdlog.h>
namespace logger = spdlog;

#include "archive_sys.hpp"

#include <archive.h>
#include <filesystem>
#include <memory>

bool xwim::ArchiveEntrySys::is_empty() {
  return (this->ae.get() == nullptr);
}

std::string xwim::ArchiveEntrySys::path_name() {
  return archive_entry_pathname(this->ae.get());
}

std::filesystem::path xwim::ArchiveEntrySys::path() {
  return std::filesystem::path{this->path_name()};
}

mode_t xwim::ArchiveEntrySys::file_type() {
  return archive_entry_filetype(this->ae.get());
}

bool xwim::ArchiveEntrySys::is_directory() {
  return S_ISDIR(this->file_type());
}

xwim::ArchiveReaderSys::ArchiveReaderSys(std::filesystem::path& path) {
  int r;  // libarchive error handling

  logger::trace("Setting up archive reader");
  this->ar = archive_read_new();
  archive_read_support_filter_all(this->ar);
  archive_read_support_format_all(this->ar);

  logger::trace("Reading archive at {}", path.c_str());
  r = archive_read_open_filename(this->ar, path.c_str(), 10240);
  if (r != ARCHIVE_OK)
    throw ArchiveSysException{"Could not open archive file", this->ar};

  logger::trace("Archive read succesfully");
}

xwim::ArchiveReaderSys::~ArchiveReaderSys() {
  archive_free(this->ar);
}

xwim::ArchiveEntrySys& xwim::ArchiveReaderSys::next() {
  int r;  // libarchive error handling
  logger::trace("Listing next archive entry");
  archive_entry* ae;
  r = archive_read_next_header(this->ar, &ae);
  if (r != ARCHIVE_OK)
    throw(ArchiveSysException{"Could not list archive", this->ar});

  this->cur_entry = xwim::ArchiveEntrySys { ae };


  logger::trace("Got archive header");

  return this->cur_entry;
}

xwim::ArchiveEntrySys& xwim::ArchiveReaderSys::cur() {
  return this->cur_entry;
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys(std::filesystem::path& root) {
    std::filesystem::create_directories(root);
    std::filesystem::current_path(root);

    this->writer = archive_write_disk_new();
    archive_write_disk_set_standard_lookup(this->writer);
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys() {
  this->writer = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(this->writer);
}

void xwim::ArchiveExtractorSys::extract_all(xwim::ArchiveReaderSys& reader) {
  for(;;) {
    ArchiveEntrySys& entry = reader.next();

    if(entry.is_empty()) break;

    this->extract(reader, entry);
  }
}

static int copy_data(struct archive* ar, struct archive* aw) {
  int r;
  const void* buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r != ARCHIVE_OK) {
      return (r);
    }
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK) {
      return (r);
    }
  }
}

void xwim::ArchiveExtractorSys::extract(xwim::ArchiveReaderSys& reader, xwim::ArchiveEntrySys& entry) {
  int r;
  r = archive_write_header(this->writer, entry.ae.get());
  if (r != ARCHIVE_OK) {
    throw(ArchiveSysException("Could not extract entry", reader.ar));
  }

  r = copy_data(reader.ar, this->writer);
  if (r != ARCHIVE_OK) {
    throw(ArchiveSysException("Could not extract entry", reader.ar));
  }
}
