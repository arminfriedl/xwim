#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "../Archiver.hpp"
#include "../util/Common.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

static int copy_data(shared_ptr<archive> reader, shared_ptr<archive> writer);

void LibArchiver::compress(set<fs::path> ins, fs::path archive_out) {
  spdlog::debug("Compressing to {}", archive_out);
  int r;  // libarchive error handling
  static char buff[16384]; // read buffer

  // cannot use unique_ptr here since unique_ptr requires a
  // complete type. `archive` is forward declared only.
  shared_ptr<archive> writer;
  writer = shared_ptr<archive>(archive_write_new(), archive_write_free);
//  archive_write_add_filter_gzip(writer.get());
//  archive_write_set_format_pax_restricted(writer.get());
  archive_write_set_format_filter_by_ext(writer.get(), archive_out.c_str());
  archive_write_open_filename(writer.get(), archive_out.c_str());

  shared_ptr<archive> reader;

  shared_ptr<archive_entry> entry = shared_ptr<archive_entry>(archive_entry_new(), archive_entry_free);

  for (auto in : ins) {
    spdlog::debug("Compressing {}", in);
    reader = shared_ptr<archive>(archive_read_disk_new(), archive_read_free);
    archive_read_disk_set_standard_lookup(reader.get());

    r = archive_read_disk_open(reader.get(), in.c_str());
    if (r != ARCHIVE_OK) {
      throw XwimError{"Failed opening {}. {}", in,
                      archive_error_string(reader.get())};
    }

    for (;;) {
      r = archive_read_next_header2(reader.get(), entry.get());

      if (r == ARCHIVE_EOF) break;

      if (r != ARCHIVE_OK) {
        throw XwimError{"Failed compressing archive entry. {}",
                        archive_error_string(reader.get())};
      }

      spdlog::debug("Adding {} to archive", archive_entry_pathname(entry.get()));
      r = archive_write_header(writer.get(), entry.get());
      if (r != ARCHIVE_OK) {
        throw XwimError{"Failed writing archive entry. {}",
                        archive_error_string(writer.get())};
      }

      /* For now, we use a simpler loop to copy data
       * into the target archive. */
      int fd = open(archive_entry_sourcepath(entry.get()), O_RDONLY);
      ssize_t len = read(fd, buff, sizeof(buff));
      while (len > 0) {
        archive_write_data(writer.get(), buff, len);
        len = read(fd, buff, sizeof(buff));
      }
      close(fd);

      archive_entry_clear(entry.get());
      archive_read_disk_descend(reader.get());
    }
  }
}

void LibArchiver::extract(fs::path archive_in, fs::path out) {
  spdlog::debug("Extracting archive {} to {}", archive_in, out);
  int r;  // libarchive error handling

  // cannot use unique_ptr here since unique_ptr requires a
  // complete type. `archive` is forward declared only.
  shared_ptr<archive> reader;
  reader = shared_ptr<archive>(archive_read_new(), archive_read_free);
  archive_read_support_filter_all(reader.get());
  archive_read_support_format_all(reader.get());
  r = archive_read_open_filename(reader.get(), archive_in.c_str(), 10240);
  if (r != ARCHIVE_OK) {
    throw XwimError{"Failed opening archive {}. {}", archive_in,
                    archive_error_string(reader.get())};
  }

  shared_ptr<archive> writer;
  writer = shared_ptr<archive>(archive_write_disk_new(), archive_write_free);
  archive_write_disk_set_standard_lookup(writer.get());

  fs::create_directories(out);
  fs::path cur_path = fs::current_path();
  fs::current_path(out);

  archive_entry *entry;
  for (;;) {
    r = archive_read_next_header(reader.get(), &entry);
    if (r == ARCHIVE_EOF) break;

    if (r != ARCHIVE_OK) {
      throw XwimError{"Failed extracting archive entry. {}",
                      archive_error_string(reader.get())};
    }

    r = archive_write_header(writer.get(), entry);
    if (r != ARCHIVE_OK) {
      throw XwimError{"Failed writing archive entry header. {}",
                      archive_error_string(writer.get())};
    }

    if (archive_entry_size(entry) > 0) {
      r = copy_data(reader, writer);
      if (r != ARCHIVE_OK) {
        throw XwimError{"Failed writing archive entry data. {}",
                        archive_error_string(writer.get())};
      }
    }

    r = archive_write_finish_entry(writer.get());
    if (r != ARCHIVE_OK) {
      throw XwimError{"Failed finishing archive entry data. {}",
                      archive_error_string(writer.get())};
    }
  }

  if (r != ARCHIVE_OK && r != ARCHIVE_EOF) {
    throw XwimError{"Failed extracting archive {}. {}", archive_in,
                    archive_error_string(reader.get())};
  }

  fs::current_path(cur_path);
}

static int copy_data(shared_ptr<archive> reader, shared_ptr<archive> writer) {
  int r;
  const void *buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(reader.get(), &buff, &size, &offset);
    if (r == ARCHIVE_EOF) {
      return (ARCHIVE_OK);
    }
    if (r != ARCHIVE_OK) {
      return (r);
    }
    r = archive_write_data_block(writer.get(), buff, size, offset);
    if (r != ARCHIVE_OK) {
      return (r);
    }
  }
}

}  // namespace xwim
