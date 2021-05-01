#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/StdOutput.h>
#include <tclap/SwitchArg.h>
#include <tclap/UnlabeledMultiArg.h>
#include <tclap/ValueArg.h>

#include <filesystem>

template <>
struct TCLAP::ArgTraits<std::filesystem::path> {
  typedef ValueLike ValueCategory;
};

namespace xwim {

namespace fs = std::filesystem;

class Opt {
 private:
  // clang-format off
  TCLAP::CmdLine cmd_line
    {"xwim - Do What I Mean Extractor", ' ', "0.5.0"};

  TCLAP::SwitchArg arg_compress
    {"c", "compress", "Compress <files>", cmd_line, false};

  TCLAP::SwitchArg arg_extract
    {"x", "extract", "Extract <file>", cmd_line, false};

  TCLAP::ValueArg<fs::path> arg_out
    {"o", "out", "Out <path>", false, fs::path{}, "A path on the filesystem", cmd_line};

  TCLAP::UnlabeledMultiArg<fs::path> arg_paths
    {"Paths", "Filesystem paths to extract or compress", true, "A path on the filesystem", cmd_line};
  // clang-format on

 public:
  void parse(int argc, char** argv) { cmd_line.parse(argc, argv); }

  void validate() {
    if (arg_extract.isSet() && arg_compress.isSet()) {
      // This is a bit ugly but `none-or-xor` only available in
      // tclap-1.4 which is not well supported in current
      // distributions
      auto out = TCLAP::StdOutput{};
      TCLAP::ArgException e{
          "Cannot compress `-c` and extract `-x` simultaneously"};

      try {
        out.failure(cmd_line, e);
      } catch (TCLAP::ExitException& e) {
        exit(e.getExitStatus());
      }
    }
  }
};

}  // namespace xwim
