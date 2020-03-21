#include <archive_entry.h>
#include <spdlog/spdlog.h>
namespace logger = spdlog;

#include "archive_sys.hpp"

#include <archive.h>
#include <filesystem>
#include <memory>

bool xwim::ArchiveEntryView::is_empty() {
  return (this->ae == nullptr);
}

std::string xwim::ArchiveEntryView::path_name() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};

  return archive_entry_pathname(this->ae);
}

std::filesystem::path xwim::ArchiveEntryView::path() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};
  return std::filesystem::path{this->path_name()};
}

mode_t xwim::ArchiveEntryView::file_type() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};
  return archive_entry_filetype(this->ae);
}

bool xwim::ArchiveEntryView::is_directory() {
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
  logger::trace("Destructing ArchiveReaderSys");

  if (this->ar) archive_read_free(this->ar);
}

bool xwim::ArchiveReaderSys::advance() {
  int r;  // libarchive error handling
  logger::trace("Advancing reader to next archive entry");

  r = archive_read_next_header(this->ar, &this->ae);
  if (r == ARCHIVE_EOF) { this->ae = nullptr; return false; }
  if (r != ARCHIVE_OK) throw(ArchiveSysException{"Could not list archive", this->ar});

  logger::trace("Got entry {}", archive_entry_pathname(ae));
  return true;
}

const xwim::ArchiveEntryView xwim::ArchiveReaderSys::cur() {
  return ArchiveEntryView{this->ae};
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys(std::filesystem::path& root) {
  logger::trace("Constructing ArchiveExtractorSys with path {}", root.string());

  std::filesystem::create_directories(root);
  std::filesystem::current_path(root);

  this->writer = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(this->writer);

  logger::trace("Constructed ArchiveExtractorSys at {:p}", (void*) this->writer);
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys() {
  logger::trace("Construction ArchiveExtractorSys without root");

  this->writer = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(this->writer);
  logger::trace("Constructed ArchiveExtractorSys at {:p}", (void*) this->writer);
}

void xwim::ArchiveExtractorSys::extract_all(xwim::ArchiveReaderSys& reader) {
  while(reader.advance()) {
    this->extract_entry(reader);
  }
}

// forward declared
static int copy_data(struct archive* ar, struct archive* aw);

void xwim::ArchiveExtractorSys::extract_entry(xwim::ArchiveReaderSys& reader) {
  int r;
  r = archive_write_header(this->writer, reader.ae);
  if (r != ARCHIVE_OK) {
    throw(ArchiveSysException("Could not extract entry", reader.ar));
  }

  r = copy_data(reader.ar, this->writer);
  if (r != ARCHIVE_OK) {
    throw(ArchiveSysException("Could not extract entry", reader.ar));
  }
}

xwim::ArchiveExtractorSys::~ArchiveExtractorSys(){
  logger::trace("Destructing ArchiveExtractorSys at {:p}", (void*) this->writer);
  if(this->writer) {
    archive_write_close(this->writer);
    archive_write_free(this->writer);
  }
}

static int copy_data(struct archive* ar, struct archive* aw) {
  int r;
  const void* buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF) {
      return (ARCHIVE_OK);
    }
    if (r != ARCHIVE_OK) {
      return (r);
    }
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK) {
      return (r);
    }
  }
}
