#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <exception>
#include <memory>
#include <set>
#include <stdexcept>

#include "Common.hpp"
#include "Opt.hpp"
#include "Archiver.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

Xwim xwim(UserOpt user_opt);

class Xwim {
 private:
  Action action;
  fs::path out;
  set<fs::path> ins;
  unique_ptr<Archiver> archiver;

  void infer_action();
  void infer_output();
  void infer_compression_output();
  void infer_extraction_output();
  void sanitize_output();

 public:
  Xwim();

  void try_infer();
  void dwim();

  void setCompress();
  void setExtract();
  void setOut(fs::path);
  void setIns(vector<fs::path> ins);
};

}  // namespace xwim
