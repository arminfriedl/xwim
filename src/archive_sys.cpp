#include <archive_entry.h>
#include <fcntl.h>
#include <spdlog/spdlog.h>

#include "archive.hpp"
#include "fileformats.hpp"
#include "spec.hpp"
namespace logger = spdlog;

#include <archive.h>

#include <filesystem>
#include <memory>

#include "archive_sys.hpp"

namespace fs = std::filesystem;

bool xwim::ArchiveEntryView::is_empty() { return (this->ae == nullptr); }

std::string xwim::ArchiveEntryView::path_name() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};

  return archive_entry_pathname(this->ae);
}

fs::path xwim::ArchiveEntryView::path() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};
  return fs::path{this->path_name()};
}

mode_t xwim::ArchiveEntryView::file_type() {
  if (!this->ae) throw ArchiveSysException{"Access to invalid archive entry"};
  return archive_entry_filetype(this->ae);
}

bool xwim::ArchiveEntryView::is_directory() {
  return S_ISDIR(this->file_type());
}

xwim::ArchiveReaderSys::ArchiveReaderSys(fs::path &path) {
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
  if (r == ARCHIVE_EOF) {
    this->ae = nullptr;
    return false;
  }
  if (r != ARCHIVE_OK)
    throw(ArchiveSysException{"Could not list archive", this->ar});

  logger::trace("Got entry {}", archive_entry_pathname(ae));
  return true;
}

const xwim::ArchiveEntryView xwim::ArchiveReaderSys::cur() {
  return ArchiveEntryView{this->ae};
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys(fs::path &root) {
  logger::trace("Constructing ArchiveExtractorSys with path {}", root.string());

  fs::create_directories(root);
  fs::current_path(root);

  this->writer = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(this->writer);

  logger::trace("Constructed ArchiveExtractorSys at {:p}",
                (void *)this->writer);
}

xwim::ArchiveExtractorSys::ArchiveExtractorSys() {
  logger::trace("Construction ArchiveExtractorSys without root");

  this->writer = archive_write_disk_new();
  archive_write_disk_set_standard_lookup(this->writer);
  logger::trace("Constructed ArchiveExtractorSys at {:p}",
                (void *)this->writer);
}

void xwim::ArchiveExtractorSys::extract_all(xwim::ArchiveReaderSys &reader) {
  while (reader.advance()) {
    this->extract_entry(reader);
  }
}

// forward declared
static int copy_data(struct archive *ar, struct archive *aw);

void xwim::ArchiveExtractorSys::extract_entry(xwim::ArchiveReaderSys &reader) {
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

xwim::ArchiveExtractorSys::~ArchiveExtractorSys() {
  logger::trace("Destructing ArchiveExtractorSys at {:p}",
                (void *)this->writer);
  if (this->writer) {
    archive_write_close(this->writer);
    archive_write_free(this->writer);
  }
}

xwim::ArchiveCompressorSys::ArchiveCompressorSys(
    fs::path &root, xwim::CompressSpec compress_spec)
    : root{root}, compress_spec{compress_spec} {
  this->new_archive = archive_write_new();

  for (xwim::archive_filter filter : this->compress_spec.filters) {
    archive_write_add_filter(this->new_archive, filter);
  }

  archive_write_set_format(this->new_archive, this->compress_spec.format);
}

// forward declared
static fs::path archive_path_norm(const fs::path &root,
                                  const xwim::CompressSpec &compress_spec);

void xwim::ArchiveCompressorSys::compress() {
  fs::path archive_path = archive_path_norm(this->root, this->compress_spec);

  logger::debug("Writing archive at: {}", archive_path.filename().c_str());

  archive_write_open_filename(this->new_archive,
                              archive_path.filename().c_str());

  archive *disk = archive_read_disk_new();
  archive_read_disk_set_standard_lookup(disk);

  int r;

  r = archive_read_disk_open(disk, fs::relative(this->root).c_str());
  if (r != ARCHIVE_OK) {
    throw ArchiveSysException("Could not open path for archiving", disk);
  }

  archive_entry *entry;
  char buff[16384];

  for (;;) {
    entry = archive_entry_new();
    r = archive_read_next_header2(disk, entry);
    if (r == ARCHIVE_EOF) break;
    if (r != ARCHIVE_OK) {
      throw ArchiveSysException("Could not read next archive entry", disk);
    }

    archive_read_disk_descend(disk);

    const char* ae_path = archive_entry_pathname(entry);
    fs::path ae_rel_path = fs::relative(fs::path(ae_path), this->root.parent_path());
    archive_entry_set_pathname(entry,  ae_rel_path.c_str());
    logger::trace("Processing entry {}", archive_entry_pathname(entry));

    r = archive_write_header(this->new_archive, entry);
    if (r < ARCHIVE_OK) {
      throw ArchiveSysException("Could not write header for archive entry",
                                this->new_archive);
    }

    if (r > ARCHIVE_FAILED) {
      int fd = open(archive_entry_sourcepath(entry), O_RDONLY);
      ssize_t len = read(fd, buff, sizeof(buff));
      while (len > 0) {
        archive_write_data(this->new_archive, buff, len);
        len = read(fd, buff, sizeof(buff));
      }
      close(fd);
    }
    logger::trace("Entry written {}", archive_entry_pathname(entry));
    archive_entry_free(entry);
  }
}

xwim::ArchiveCompressorSys::~ArchiveCompressorSys() {
  logger::trace("Destructing ArchiveExtractorSys at {:p}",
                (void *)this->new_archive);
  if (this->new_archive) {
    archive_write_close(this->new_archive);
    archive_write_free(this->new_archive);
  }
}

/** Creates an archive path from the path to compress and normalizes it
 *
 *  Note that currently only single arguments are allowed for `xwim` to
 * minimize ambiguity.
 *
 *  The archive path is determined from the argument file/directory by:
 *  1. If file:
 *    1.1. Stem the filename
 *    1.2. Append an extension appropriate for the archive format (from the
 * spec)
 *  2. If directory:
 *    2.1. Remove any trailing '/'
 *    2.2. Append an extension appropriate for the archive format (from the
 * spec)
 */
static fs::path archive_path_norm(const fs::path &root,
                                  const xwim::CompressSpec &compress_spec) {
  fs::path archive_path{root};
  fs::file_status archive_path_stat = fs::status(archive_path);

  std::set known_types = {fs::file_type::directory, fs::file_type::regular};
  fs::perms flag_mask =
      fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read;

  if (!fs::exists(archive_path)) {
    logger::error("Non-existing path: {}", archive_path.string());
    throw xwim::ArchiveSysException{"Path does not exists"};
  }

  if (!known_types.count(archive_path_stat.type())) {
    logger::error("Unknown path type: {}", archive_path_stat.type());
    throw xwim::ArchiveSysException{"Unknown path type"};
  }

  if ((archive_path_stat.permissions() & flag_mask) == fs::perms::none) {
    logger::error("Cannot read path with permissions: {}",
                  archive_path_stat.permissions());
    throw xwim::ArchiveSysException{"Unreadable path"};
  }

  if (archive_path_stat.type() == fs::file_type::regular) {
    while (archive_path.has_extension()) {
      archive_path.replace_extension();
    }
  }

  if (archive_path_stat.type() == fs::file_type::directory) {
    if (archive_path.string().back() == '/') {
      logger::trace("Found trailing / in path");
      std::string ps = archive_path.string();
      ps.erase(ps.size() - 1, 1);

      archive_path = fs::path{ps};
      logger::trace("Normalized path to {}", archive_path.string());
    }
  }

  archive_path.concat(compress_spec.extension);
  return archive_path;
}

static int copy_data(struct archive *ar, struct archive *aw) {
  int r;
  const void *buff;
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
