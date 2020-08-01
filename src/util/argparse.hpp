#pragma once

#include <filesystem>
#include <ostream>
#include <sstream>

#include "../fileformats.hpp"

namespace xwim {
/**
 * xwim allows for
 * 1. an archive
 * 2. a file or folder
 *
 * In case of (1) the archive will be extracted according to the xwim
 * do-what-i-mean rules.
 *
 * In case of (2) the file or folder will be compressed into a "platform native"
 * format, i.e. what appears to be the most widely used format on that platform.
 * In case of unix this is tar.gz. In case of windows this is zip. The archive
 * gets the same name as the file or folder and a proper extension.
 *
 * A list of files or folders is unsupported as it would be too ambigious to
 * choose a name for the archive. A list of archives is unsupported for
 * consistency reasons. Any mixture is unsupported as it would be too ambigious
 * what the user wants. This is subject to change in the future.
 */
namespace argparse {

class XwimPath {
 private:
  std::filesystem::path _path;

 public:
  XwimPath() : _path{} {};
  XwimPath(int argc, char** argv);

  bool is_archive();
  std::filesystem::path path() const;
};

class ArgParseException : public std::exception {
 private:
  std::string _what;

 public:
  ArgParseException(std::string what) : _what{what} {};

  virtual const char* what() const noexcept { return this->_what.c_str(); }
};

XwimPath parse(int argc, char** argv);

inline std::string usage() {
  std::stringstream s;
  s << "USAGE:"
    << "\t xwim <path>\n"
    << "\n"

    << "PARAMS:" << std::left << std::setfill('.') << std::setw(10)
    << "\t path "
    << " Archive\n"
    << "\n"

    << "FORMATS:\n"
    << "\t .7z, .7zip .jar, .tgz, .bz2, .bzip2\n"
    << "\t .gz, .gzip, .rar, .tar, .xz, .zip\n"
    << "\n"

    << "EXAMPLES:\n"
    << "\t Extract archive archive.tar.gz:\n"
    << "\t xwim archive.tar.gz\n"
    << std::endl;

  return s.str();
}

}  // namespace argparse
}  // namespace xwim
