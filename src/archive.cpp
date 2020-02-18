#include <spdlog/spdlog.h>
#include <sys/stat.h>
namespace logger = spdlog;

#include <archive.h>
#include <archive_entry.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "archive.hpp"
#include "spec.hpp"

namespace xwim {

static archive_entry* _archive_next_entry(archive* archive) {
  int r;  // libarchive error handling
  archive_entry* entry;
  logger::trace("Listing next archive entry");
  r = archive_read_next_header(archive, &entry);
  if (r != ARCHIVE_OK)
    throw(ArchiveException{"Could not list archive", archive});

  logger::trace("Got archive header");
  return entry;
}

static void _spec_is_root_filename(ArchiveSpec* spec,
                                   archive_entry* entry,
                                   std::filesystem::path* filepath) {
  std::filesystem::path entry_path{archive_entry_pathname(entry)};
  std::filesystem::path norm_stem = filepath->filename();

  while (norm_stem.has_extension())
    norm_stem = norm_stem.stem();

  if (*entry_path.begin() != norm_stem) {
    logger::debug("Archive root does not match archive name");
    spec->is_root_filename = false;
  } else {
    logger::debug("Archive root matches archive name");
    spec->is_root_filename = true;
  }
  logger::debug("\t-> Archive root: {}", entry_path.begin()->string());
  logger::debug("\t-> Archive stem: {}", norm_stem.string());
}

static void _spec_is_root_dir(ArchiveSpec* spec, archive_entry* entry) {
  if (S_ISDIR(archive_entry_filetype(entry))) {
    logger::debug("Archive root is directory");
    spec->is_root_dir = true;
  } else {
    logger::debug("Archive root is not a directory");
    spec->is_root_dir = false;
  }
  logger::debug("\t-> Archive mode_t: {0:o}", archive_entry_filetype(entry));
}

static void _spec_has_single_root(ArchiveSpec* spec,
                                  archive_entry* first_entry,
                                  archive* archive) {
  std::filesystem::path first_entry_root =
      *(std::filesystem::path{archive_entry_pathname(first_entry)}.begin());
  logger::trace("Testing roots");

  spec->has_single_root = true;
  archive_entry* entry;
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    std::filesystem::path next_entry{archive_entry_pathname(entry)};
    logger::trace("Path: {}, Root: {}", next_entry.string(),
                  next_entry.begin()->string());

    if (first_entry_root != *next_entry.begin()) {
      logger::debug("Archive has multiple roots");
      logger::debug("\t-> Archive root I: {}",
                    first_entry_root.begin()->string());
      logger::debug("\t-> Archive root II: {}", next_entry.begin()->string());

      spec->has_single_root = false;
      break;
    }
  }
  if (spec->has_single_root)
    logger::debug("Archive has single root: {}", first_entry_root.string());
}

Archive::Archive(std::filesystem::path path) : path{path} {
  int r;  // libarchive error handling

  logger::trace("Setting up archive reader");
  this->xwim_archive = archive_read_new();
  archive_read_support_filter_all(this->xwim_archive);
  archive_read_support_format_all(this->xwim_archive);

  logger::trace("Reading archive at {}", path.c_str());
  r = archive_read_open_filename(this->xwim_archive, path.c_str(), 10240);
  if (r != ARCHIVE_OK)
    throw ArchiveException{"Could not open archive file", this->xwim_archive};

  logger::trace("Archive read succesfully");
}

Archive::~Archive() {
  archive_read_free(this->xwim_archive);
}

ArchiveSpec Archive::check() {
  logger::trace("Creating archive spec for {}", this->path.string());

  ArchiveSpec archive_spec;

  archive_entry* first_entry = _archive_next_entry(this->xwim_archive);
  if (first_entry == nullptr) {  // archive is empty
    logger::debug("Archive is empty");
    return {false, false, false};
  }

  logger::trace("Found archive entry {}", archive_entry_pathname(first_entry));

  _spec_is_root_filename(&archive_spec, first_entry, &this->path);
  _spec_is_root_dir(&archive_spec, first_entry);
  _spec_has_single_root(&archive_spec, first_entry, this->xwim_archive);

  return archive_spec;
}

void Archive::extract(ExtractSpec extract_spec) {}

}  // namespace xwim
