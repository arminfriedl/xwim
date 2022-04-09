#include "XwimIntent.hpp"

#include <spdlog/spdlog.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/StdOutput.h>
#include <tclap/SwitchArg.h>
#include <tclap/UnlabeledMultiArg.h>
#include <tclap/ValueArg.h>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>

#include "Archiver.hpp"

template <>
struct::TCLAP::ArgTraits<std::filesystem::path> {
  // `operator=` here for path construction because `operator>>`
  // (`ValueLike`) causes a split at whitespace
  typedef StringLike ValueCategory;
};

namespace xwim {

void UserOpt::parse_args(int argc, char** argv) {
  // clang-format off
  // TODO: read version from -DVERSION during compilation
  TCLAP::CmdLine cmd {"xwim - Do What I Mean Extractor", ' ', "0.3.0"};

  TCLAP::SwitchArg arg_compress
    {"c", "compress", "Compress <files>", cmd, false};

  TCLAP::SwitchArg arg_extract
    {"x", "extract", "Extract <file>", cmd, false};

  TCLAP::SwitchArg arg_noninteractive
    {"i", "non-interactive", "Non-interactive, fail on ambiguity", cmd, false};

  TCLAP::ValueArg<path> arg_outfile
    {"o", "out", "Out <file-or-path>", false, path{}, "A path on the filesystem", cmd};

  TCLAP::UnlabeledMultiArg<path> arg_paths
    {"files", "Archive to extract or files to compress", true, "A path on the filesystem", cmd};
  // clang-format on

  // TODO: ideally we'd make sure during parsing that compress and extract
  // cannot both be true

  cmd.parse(argc, argv);

  // clang-format off

  // Only set things if they are actually parsed from args. Otherwise we'd
  // override settings set through other means, e.g. config files
  if (arg_compress.isSet())       { this->compress    = arg_compress.getValue();        }
  if (arg_extract.isSet())        { this->extract     = arg_extract.getValue();         }
  if (arg_noninteractive.isSet()) { this->interactive = !arg_noninteractive.getValue(); }
  if (arg_outfile.isSet())        { this->out         = arg_outfile.getValue();         }
  if (arg_paths.isSet())          { this->paths       = arg_paths.getValue();           }

  // clang-format on
}

void UserOpt::parse_config(path config) {  // TODO
  spdlog::warn("Config parsing is not implemented");
  return;
}

UserIntent UserOpt::guess_intent() {
  return UserIntent{action_intent(), out_intent(), paths_intent()};
}

Action UserOpt::action_intent() {
  if (compress && extract) {
    throw XwimError("Cannot compress and extract simultaneously");
  }

  if (compress) return Action::COMPRESS;
  if (extract) return Action::EXTRACT;

  bool can_extract_all = std::all_of(
      paths.begin(), paths.end(), [](path path) { return can_extract(path); });

  if (can_extract_all && !out) {
    return Action::EXTRACT;
  }  // else if can_extract_all && !is_archive(out) -> EXTRACT

  if (!can_extract_all && out /* && is_archive(out) */) {
    return Action::COMPRESS;
  }

  if (interactive) {
    std::cout << "Do you want to compress (y/n)? [y] ";
    char c;
    std::cin >> c;

    if (c != 'y' && c != 'n' && c != '\n') {
      throw XwimError("Cannot guess action. Please answer 'y' or 'n'.");
    }

    if (c == 'y' || c == '\n') {
      return Action::COMPRESS;
    } else if (c == 'n') {
      return Action::EXTRACT;
    }
  }

  throw XwimError("Cannot guess action (compress/extract)");
}

path UserOpt::out_intent() {

}

set<path> UserOpt::paths_intent() {

}

}  // namespace xwim
