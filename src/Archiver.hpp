#pragma once

#include <fmt/core.h>

#include <filesystem>
#include <map>
#include <memory>
#include <set>

#include "util/Common.hpp"

namespace xwim {

// Invariant:
// `extensions_format` defines a surjection from `format_extensions`
// to `Formats`
const std::set<std::string> format_extensions{".tar.gz", ".zip"};
enum class Format { TAR_GZ, ZIP };
const std::map<std::string, Format> extensions_format{
    {".tar.gz", Format::TAR_GZ}, {".zip", Format::ZIP}};

class Archiver {
 public:
  virtual void compress(std::set<std::filesystem::path> ins,
                        std::filesystem::path archive_out) = 0;

  virtual void extract(std::filesystem::path archive_in,
                       std::filesystem::path out) = 0;

  virtual ~Archiver() = default;
};

class LibArchiver : public Archiver {
 public:
  void compress(std::set<std::filesystem::path> ins,
                std::filesystem::path archive_out);

  void extract(std::filesystem::path archive_in, std::filesystem::path out);
};

std::filesystem::path archive_extension(const std::filesystem::path& path);
std::filesystem::path strip_archive_extension(const std::filesystem::path& path);
std::filesystem::path default_archive(const std::filesystem::path& base);

Format parse_format(const std::filesystem::path& path);
bool can_handle_archive(const std::filesystem::path& path);

std::unique_ptr<Archiver> make_archiver(const std::string& archive_name);

}  // namespace xwim
