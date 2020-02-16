#include <iostream>
#include <ostream>
#include <string>

#include "archive.hpp"
#include "spec.hpp"

int main(int argc, char** argv) {
  std::string filename {argv[1]};
  xwim::Archive archive {filename};

  xwim::ArchiveSpec asp = archive.check();

  std::cout << "Has subarch: " << asp.has_subarchive
            << "Is root: " << asp.is_root_dir
            << "Is root dir filename: " << asp.is_root_dir_filename
            << std::endl;

  archive.extract(xwim::ExtractSpec {.make_dir=true, .dirname="Test"});
}
