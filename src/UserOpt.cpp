#include "UserOpt.hpp"

#include <tclap/CmdLine.h>

template <>
struct TCLAP::ArgTraits<std::filesystem::path> {
  // We use `operator=` here for path construction
  // because `operator>>` (`ValueLike`) causes a split at
  // whitespace
  typedef StringLike ValueCategory;
};

namespace xwim {
UserOpt::UserOpt(int argc, char** argv) {
  // clang-format off
  TCLAP::CmdLine cmd
    {"xwim - Do What I Mean Extractor", ' ', "0.3.0"};

  TCLAP::SwitchArg arg_compress
    {"c", "compress", "Compress <files>", cmd, false};

  TCLAP::SwitchArg arg_extract
    {"x", "extract", "Extract <file>", cmd, false};

  TCLAP::SwitchArg arg_noninteractive
    {"i", "non-interactive", "Non-interactive, fail on ambiguity", cmd, false};

  TCLAP::ValueArg<fs::path> arg_outfile
    {"o", "out", "Out <file-or-path>", false, fs::path{}, "A path on the filesystem", cmd};

  TCLAP::MultiSwitchArg arg_verbose
    {"v", "verbose", "Verbosity level", cmd, 0};

  TCLAP::UnlabeledMultiArg<fs::path> arg_paths
    {"files", "Archive(s) to extract or file(s) to compress", true, "A path on the filesystem", cmd};
  // clang-format on

  cmd.parse(argc, argv);

  if (arg_compress.isSet()) this->compress = arg_compress.getValue();
  if (arg_extract.isSet()) this->extract = arg_extract.getValue();
  if (arg_outfile.isSet()) this->out = arg_outfile.getValue();

  this->verbosity = arg_verbose.getValue();
  this->interactive = !arg_noninteractive.getValue();

  if (arg_paths.isSet()) {
    this->paths =
        set<fs::path>{arg_paths.getValue().begin(), arg_paths.getValue().end()};
  }
}
}  // namespace xwim
