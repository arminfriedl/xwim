#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/StdOutput.h>
#include <tclap/SwitchArg.h>
#include <tclap/UnlabeledMultiArg.h>
#include <tclap/ValueArg.h>

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>

#include "Common.hpp"
#include "Log.hpp"
#include "Opt.hpp"
#include "Xwim.hpp"

using namespace xwim;
using namespace std;

template <>
struct TCLAP::ArgTraits<std::filesystem::path> {
  typedef ValueLike ValueCategory;
};

UserOpt parse_args(int argc, char** argv) {
  // clang-format off
  TCLAP::CmdLine cmd_line
    {"xwim - Do What I Mean Extractor", ' ', "0.5.0"};

  TCLAP::SwitchArg arg_compress
    {"c", "compress", "Compress <files>", cmd_line, false};

  TCLAP::SwitchArg arg_extract
    {"x", "extract", "Extract <files>", cmd_line, false};

  TCLAP::SwitchArg arg_noninteractive
    {"ni", "noninteractive", "Fail if action cannot be determined", cmd_line, false};

  TCLAP::ValueArg<std::filesystem::path> arg_out
    {"o", "out", "Out <path>", false, std::filesystem::path{}, "A path on the filesystem", cmd_line};

  TCLAP::UnlabeledMultiArg<std::filesystem::path> arg_paths
    {"Paths", "Filesystem paths to extract or compress", true, "A path on the filesystem", cmd_line};
  // clang-format on

  cmd_line.parse(argc, argv);

  UserOpt user_opt;

  if(arg_compress.getValue()) user_opt.action = Action::COMPRESS;
  if(arg_extract.getValue()) user_opt.action = Action::EXTRACT;
  if(arg_out.isSet()) user_opt.out = arg_out.getValue();
  user_opt.interactive = !arg_noninteractive.getValue();
  user_opt.paths = std::set<fs::path>{arg_paths.getValue().begin(), arg_paths.getValue().end()};

  return user_opt;
}

int main(int argc, char** argv) {
  log::init();

  UserOpt user_opt = parse_args(argc, argv);
  XwimConfig xwim_config = guess_wim(user_opt);
  do_wim(xwim_config);
}
