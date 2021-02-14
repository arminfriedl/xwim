#include "argparse.hpp"

#include <fmt/core.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/SwitchArg.h>
#include <tclap/UnlabeledMultiArg.h>
#include <tclap/ValueArg.h>

#include <filesystem>
#include <iostream>
#include <vector>

#include "archivinfo.hpp"
#include "fileformats.hpp"

using namespace TCLAP;
using namespace xwim;
namespace fs = std::filesystem;

template <>
struct TCLAP::ArgTraits<fs::path> {
  typedef ValueLike ValueCategory;
};

ArgParse::ArgParse()
    : cmd{"xwim - Do What I Mean Extractor", ' ', "0.3.0"},
      arg_compress{"c", "compress", "Compress <files>", false},
      arg_extract{"x", "extract", "Extract <file>", false},
      arg_outfile{"o",   "out",      "Out <file-or-path>",
                  false, fs::path{}, "A path on the filesystem"},
      arg_infiles{"Files", "Archive to extract or files to compress", true,
                  "A path on the filesystem"} {
  cmd.xorAdd(arg_compress, arg_extract);
  cmd.add(arg_outfile);
  cmd.add(arg_infiles);
};

void ArgParse::parse(int argc, char** argv) {
  try {
    cmd.parse(argc, argv);
  } catch (ArgException& e) {
    throw new xwim::ArgParseException(e.error());
  }

  this->extract = parse_extract();
  this->outfile = arg_outfile.getValue();
  this->infiles = arg_infiles.getValue();
}

bool ArgParse::parse_extract() {
  // extract/compress explicitly given; xor ensured in `cmd`
  if (this->arg_compress.getValue()) {
    return false;
  } else if (this->arg_extract.getValue()) {
    return true;
  }

  // Not explicitly given, check if we can guess from input

  // An outfile is given
  if (this->arg_outfile.isSet()) {
    // outfile looks like an archive
    if (xwim::archivinfo::has_known_extension(this->arg_outfile.getValue())) {
      return false;
    }

    // outfile is not a known archive, assume it meant as folder for extraction
    else {
      return true;
    }
  }

  // one infile which is an archive, so intention is probably to extract this
  if (this->arg_infiles.getValue().size() == 1 &&
      xwim::archivinfo::is_archive(this->arg_infiles.getValue().at(0))) {
    return true;
  }

  // all other cases, in particular multiple infiles, assume we want to compress
  return false;
}
bool ArgParse::compressp() { return !this->extract; }
bool ArgParse::extractp() { return this->extract; }
