#include <spdlog/spdlog.h>
#include <sys/stat.h>

namespace logger = spdlog;

#include <archive.h>
#include <archive_entry.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "archive_sys.hpp"
#include "archive.hpp"
#include "spec.hpp"

namespace xwim {

static void _spec_is_root_filename(ArchiveSpec* spec,
                                   ArchiveEntrySys& entry,
                                   std::filesystem::path* filepath) {
  auto entry_path = entry.path();
  auto norm_stem = filepath->filename();

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

static void _spec_is_root_dir(ArchiveSpec* spec, ArchiveEntrySys& entry) {
  if (entry.is_directory()) {
    logger::debug("Archive root is directory");
    spec->is_root_dir = true;
  } else {
    logger::debug("Archive root is not a directory");
    spec->is_root_dir = false;
  }
  logger::debug("\t-> Archive mode_t: {0:o}", entry.file_type());
}

static void _spec_has_single_root(ArchiveSpec* spec,
                                  ArchiveEntrySys& first_entry,
                                  ArchiveReaderSys& archive_reader) {
  std::filesystem::path first_entry_root = *(first_entry.path().begin());
  logger::trace("Testing roots");

  spec->has_single_root = true;

  while (true) {
    ArchiveEntrySys& entry = archive_reader.next();
    if(entry.is_empty()) break;

    auto next_entry = entry.path();
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

Archive::Archive(std::filesystem::path path) : path{path} {}

Archive::~Archive() {
  archive_read_free(this->xwim_archive);
}

ArchiveSpec Archive::check() {
  logger::trace("Creating archive spec for {}", this->path.string());

  ArchiveReaderSys archive_reader {this->path};

  ArchiveSpec archive_spec;

  ArchiveEntrySys& first_entry = archive_reader.next();

  if (first_entry.is_empty()) {  // archive is empty
    logger::debug("Archive is empty");
    return {false, false, false};
  }

  logger::trace("Found archive entry {}", first_entry.path_name());

  _spec_is_root_filename(&archive_spec, first_entry, &this->path);
  _spec_is_root_dir(&archive_spec, first_entry);
  _spec_has_single_root(&archive_spec, first_entry, archive_reader);

  return archive_spec;
}

void Archive::extract(ExtractSpec extract_spec) {
  ArchiveExtractorSys extractor;

  if(extract_spec.make_dir) {
    logger::trace("Creating extract directory {}", extract_spec.dirname.string());
    extractor = ArchiveExtractorSys{extract_spec.dirname};
  } else {
    extractor = ArchiveExtractorSys{};
  }

  ArchiveReaderSys reader{this->path};
  extractor.extract_all(reader);
}

}  // namespace xwim
