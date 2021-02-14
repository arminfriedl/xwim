#pragma once

#include <fmt/core.h>
#include <tclap/CmdLine.h>
#include <tclap/SwitchArg.h>
#include <tclap/UnlabeledMultiArg.h>
#include <tclap/ValueArg.h>

#include <filesystem>
#include <iostream>

namespace xwim {
class ArgParse {
 private:
  bool extract;
  std::filesystem::path outfile;
  std::vector<std::filesystem::path> infiles;

  TCLAP::CmdLine cmd;
  TCLAP::SwitchArg arg_compress;
  TCLAP::SwitchArg arg_extract;
  TCLAP::ValueArg<std::filesystem::path> arg_outfile;
  TCLAP::UnlabeledMultiArg<std::filesystem::path> arg_infiles;

 protected:
  bool parse_extract();

 public:
  ArgParse();
  void parse(int argc, char** argv);
  bool compressp();
  bool extractp();
};

class ArgParseException : public std::exception {
 private:
  std::string _what;

 public:
  ArgParseException(std::string what) : _what{what} {};
  template<typename... Args>
  ArgParseException(std::string fmt_string, Args&&... args) : _what{fmt::format(fmt_string, args...)} {};
  virtual const char* what() const noexcept { return this->_what.c_str(); }
};
}  // namespace xwim
