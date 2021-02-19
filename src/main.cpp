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

#include "Common.hpp"
#include "Log.hpp"
#include "Xwim.hpp"

using namespace xwim;
using namespace std;
namespace fs = std::filesystem;

template <>
struct TCLAP::ArgTraits<std::filesystem::path> {
  typedef ValueLike ValueCategory;
};

int main(int argc, char** argv) {
  log::init();

  TCLAP::CmdLine cmd{"xwim - Do What I Mean Extractor", ' ', "0.3.0"};

  TCLAP::SwitchArg arg_compress{"c", "compress", "Compress <files>", cmd,
                                false};
  TCLAP::SwitchArg arg_extract{"x", "extract", "Extract <file>", cmd, false};

  TCLAP::ValueArg<fs::path> arg_outfile{
      "o",   "out",      "Out <file-or-path>",
      false, fs::path{}, "A path on the filesystem",
      cmd};
  TCLAP::UnlabeledMultiArg<fs::path> arg_infiles{
      "Files", "Archive to extract or files to compress", true,
      "A path on the filesystem", cmd};

  Xwim xwim;

  cmd.parse(argc, argv);

  if (arg_extract.isSet() && arg_compress.isSet()) {
    // This is a bit ugly but `none-or-xor` only available in
    // tclap-1.4 which is not well supported in current
    // distributions
    auto out = TCLAP::StdOutput{};
    TCLAP::ArgException e{
        "Cannot compress `-c` and extract `-x` simultaneously"};
    try {
      out.failure(cmd, e);
    } catch (TCLAP::ExitException& e) {
      exit(e.getExitStatus());
    }
  }

  // `none-or-xor` ensured already
  if (arg_extract.isSet()) xwim.setExtract();
  if (arg_compress.isSet()) xwim.setCompress();

  if (arg_outfile.isSet()) xwim.setOut(arg_outfile.getValue());
  if (arg_infiles.isSet()) xwim.setIns(arg_infiles.getValue());

  try {
    xwim.try_infer();
    xwim.dwim();
  } catch (XwimError& e) {
    spdlog::error(e.what());
  }
}
