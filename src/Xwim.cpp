#include "Xwim.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <ios>
#include <iostream>
#include <iterator>
#include <random>
#include <string>

#include "Archiver.hpp"
#include "util/Common.hpp"

namespace xwim {
using namespace std;
namespace fs = std::filesystem;

#if defined(unix) || defined(__unix__) || defined(__unix)
std::string default_extension = ".tar.gz";
#elif defined(_win32) || defined(__win32__) || defined(__windows__)
std::string default_extension = ".zip";
#else
std::string default_extension = ".zip";
#endif

Xwim::Xwim() : action{Action::UNKNOWN} {}

void Xwim::try_infer() {
  infer_action();
  infer_output();

  if (action == Action::COMPRESS) {
    archiver = make_archiver(out.string());
  } else if (action == Action::EXTRACT) {
    // we can only handle one archive for extraction at a time.
    // Checked in `infer_extraction_output`
    archiver = make_archiver(ins.begin()->string());
  }
}
void Xwim::dwim() {
  switch (action) {
    case Action::COMPRESS:
      this->archiver->compress(ins, out);
      break;
    case Action::EXTRACT:
      this->archiver->extract(*ins.begin(), out);
      sanitize_output();
      break;
    default:
      spdlog::error("Unknown action");
  }
}

void Xwim::sanitize_output() {
  fs::path in_stripped = xwim::strip_archive_extension(*ins.begin());

  int count = 0;
  fs::directory_entry first_entry;
  for(auto& e: fs::directory_iterator(out)) {
    count++;
    if(first_entry.path().empty()) {
      first_entry = e;
    }
  }

  if (count >= 2) {
    spdlog::debug("Found multiple entries in extraction directory. Moving {} to {}", out, in_stripped);
    fs::rename(out, in_stripped);
  } else {
    if(first_entry.is_directory()) {
      spdlog::debug("Found single directory in extraction directory. Moving {} to {}",
          first_entry.path(), in_stripped);
      fs::rename(first_entry, in_stripped);
      fs::remove(out);
    } else {
      spdlog::debug(
          "Found single file in extraction directory. Moving {} to {}", out, in_stripped);
      fs::rename(out, in_stripped);
    }
  }
}

void Xwim::infer_action() {
  if (action != Action::UNKNOWN) return;

  if (ins.size() == 1 && can_extract(*ins.begin())) {
    action = Action::EXTRACT;
  } else {
    action = Action::COMPRESS;
  }
  spdlog::debug("Inferred action: {}", action);
}

void Xwim::infer_output() {
  if (!out.empty()) return;

  switch (action) {
    case Action::COMPRESS:
      infer_compression_output();
      break;
    case Action::EXTRACT:
      infer_extraction_output();
      break;
    default:
      throw XwimError{"Cannot infer output, action is unknown"};
  }

  spdlog::debug("Inferred out: {}", out.string());
}

void Xwim::infer_compression_output() {
  if (ins.size() == 1) {
    // archive name is just the name of the input with default archive
    // extension
    fs::path archive_stem = xwim::strip_archive_extension(*ins.begin());
    archive_stem += default_extension;
    out = archive_stem;
  } else {
    // We cannot guess the name of the output archive

    // TODO use readline/lineoise/editline for path completion
    cout << "Archive name: ";
    cin >> out;
    out = fs::path(out);
  }
}
void Xwim::infer_extraction_output() {
  if (ins.size() > 1) {
    throw XwimError{"Cannot extract more than one archive at a time"};
  }

  // create a temporary path for extraction
  fs::path archive_stem = xwim::strip_archive_extension(*ins.begin());

  // note: we use here what is considered an `extensions` by `fs::path` so that
  // we can strip it again easily later on
  archive_stem += ".";
  archive_stem += to_string(rand_int(999, 99999));
  archive_stem += ".tmp";
  this->out = archive_stem;
}

void Xwim::setCompress() {
  this->action = Action::COMPRESS;
  spdlog::debug("Set action to {}", this->action);
}
void Xwim::setExtract() {
  this->action = Action::EXTRACT;
  spdlog::debug("Set action to {}", this->action);
}
void Xwim::setOut(fs::path path) {
  this->out = path;
  spdlog::debug("Set out to {}", this->out);
}
void Xwim::setIns(vector<fs::path> ins) {
  this->ins.insert(ins.begin(), ins.end());
  if (this->ins.size() != ins.size()) {
    spdlog::warn("Duplicate input files found. Removed {} duplicate(s).",
                 (ins.size() - this->ins.size()));
  }
}
}  // namespace xwim
