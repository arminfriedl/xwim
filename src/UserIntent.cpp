#include "UserIntent.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

#include "Archiver.hpp"

namespace xwim {
unique_ptr<UserIntent> make_compress_intent(const UserOpt &userOpt) {
  if (userOpt.paths.size() == 1) {
    return make_unique<CompressSingleIntent>(
        CompressSingleIntent{*userOpt.paths.begin(), userOpt.out});
  }

  if (!userOpt.out.has_value()) {
    throw XwimError("Cannot guess output for multiple targets");
  }

  return make_unique<CompressManyIntent>(
      CompressManyIntent{userOpt.paths, userOpt.out.value()});
}

unique_ptr<UserIntent> make_extract_intent(const UserOpt &userOpt) {
  for (const path &p : userOpt.paths) {
    if (!can_handle_archive(p)) {
      throw XwimError("Cannot extract path {}", p);
    }
  }

  return make_unique<ExtractIntent>(ExtractIntent{userOpt.paths, userOpt.out});
}

unique_ptr<UserIntent> try_infer_compress_intent(const UserOpt &userOpt) {
  if (!userOpt.out.has_value()) {
    spdlog::debug("No <out> provided");
    if (userOpt.paths.size() != 1) {
      spdlog::debug(
          "Not a single-path compression. Cannot guess <out> for many-path "
          "compression");
      return nullptr;
    }

    spdlog::debug("Only one <path> provided. Assume single-path compression.");
    return make_unique<CompressSingleIntent>(
        CompressSingleIntent{*userOpt.paths.begin(), userOpt.out});
  }

  spdlog::debug("<out> provided: {}", userOpt.out.value());
  if (can_handle_archive(userOpt.out.value())) {
    spdlog::debug("{} given and a known archive format, assume compression",
                  userOpt.out.value());
    return make_compress_intent(userOpt);
  }

  spdlog::debug(
      "Cannot compress multiple paths without a user-provided output archive");
  return nullptr;
}

unique_ptr<UserIntent> try_infer_extract_intent(const UserOpt &userOpt) {
  bool can_extract_all =
      std::all_of(userOpt.paths.begin(), userOpt.paths.end(),
                  [](const path &path) { return can_handle_archive(path); });

  if (!can_extract_all) {
    spdlog::debug(
        "Cannot extract all provided <paths>. Assume this is not an "
        "extraction.");
    for (const path &p : userOpt.paths) {
      if (!can_handle_archive(p)) {
        spdlog::debug("Cannot handle {}", p);
      }
    }

    return nullptr;
  }

  if (userOpt.out.has_value() && can_handle_archive(userOpt.out.value())) {
    spdlog::debug(
        "Could extract all provided <paths>. But also {} looks like an "
        "archive. Ambiguous intent. Assume this is not an extraction.",
        userOpt.out.value());
    return nullptr;
  }

  spdlog::debug(
      "Could extract all provided <paths>. But also <out> looks like an "
      "archive. Ambiguous intent. Assume this is not an extraction.");
  return make_extract_intent(userOpt);
}

unique_ptr<UserIntent> make_intent(const UserOpt &userOpt) {
  if (userOpt.wants_compress() && userOpt.wants_extract()) {
    throw XwimError("Cannot compress and extract simultaneously");
  }
  if (userOpt.paths.empty()) {
    throw XwimError("No input given...");
  }

  // explicitly specified intent
  if (userOpt.wants_compress()) return make_compress_intent(userOpt);
  if (userOpt.wants_extract()) return make_extract_intent(userOpt);

  spdlog::info("Intent not explicitly provided, trying to infer intent");

  if (auto intent = try_infer_extract_intent(userOpt)) {
    spdlog::info("Extraction intent inferred");
    return intent;
  }
  spdlog::info("Cannot infer extraction intent");

  if (auto intent = try_infer_compress_intent(userOpt)) {
    spdlog::info("Compression intent inferred");
    return intent;
  }
  spdlog::info("Cannot infer compression intent");

  throw XwimError("Cannot guess intent");
}

void ExtractIntent::execute() {
  bool has_out = this->out.has_value();
  bool is_single = this->archives.size() == 1;

  for (const path &p : this->archives) {
    unique_ptr<Archiver> archiver = make_archiver(p);
    path out;

    if (has_out) {
      if (is_single) {  // just dump content of archive into `out`
        std::filesystem::create_directories(this->out.value());
        out = this->out.value();
      } else {  // create an `out` folder and extract inside there
        std::filesystem::create_directories(this->out.value());
        out = this->out.value() / strip_archive_extension(p);
      }
    } else {
      out = std::filesystem::current_path() / strip_archive_extension(p);
      std::filesystem::create_directories(out);
    }

    archiver->extract(p, out);

    // move folder if only one entry and that entries name is already
    // the stripped archive name
    auto dit = std::filesystem::directory_iterator(out);

    if (dit == std::filesystem::directory_iterator()) {
      spdlog::debug("Archive is empty");
    } else if (is_directory(dit->path())) {
      auto first_path = dit->path();
      auto next_entry = next(dit);

      if (next_entry == std::filesystem::directory_iterator()) {
        spdlog::debug("Archive has single entry which is a directory");
        if (std::filesystem::equivalent(first_path.filename(),
                                        out.filename())) {
          spdlog::debug("Archive entry named like archive");
          int i = rand_int(0, 100000);

          path tmp_out = path{out};
          tmp_out.concat(fmt::format(".xwim{}", i));

          spdlog::debug("Moving {} to {}", first_path, tmp_out);
          std::filesystem::rename(first_path, tmp_out);
          spdlog::debug("Removing parent {}", out);
          std::filesystem::remove(out);
          spdlog::debug("Moving {} to {}", tmp_out, out);
          std::filesystem::rename(tmp_out, out);

        } else {
          spdlog::debug("Archive entry differs from archive name");
        }
      } else {
        spdlog::debug("Archive has multiple entries");
      }
    }
  }
};

void CompressSingleIntent::execute() {
  if (this->out.has_value()) {
    if (!can_handle_archive(this->out.value())) {
      throw XwimError("Unknown archive format {}", this->out.value());
    }

    unique_ptr<Archiver> archiver = make_archiver(this->out.value());
    set<path> ins{this->in};
    archiver->compress(ins, this->out.value());
  } else {
    path out = default_archive(strip_archive_extension(this->in).stem());
    unique_ptr<Archiver> archiver = make_archiver(out);
    set<path> ins{this->in};
    archiver->compress(ins, out);
  }
};

void CompressManyIntent::execute() {
  if (!can_handle_archive(this->out)) {
    throw XwimError("Unknown archive format {}", this->out);
  }

  unique_ptr<Archiver> archiver = make_archiver(this->out);
  archiver->compress(this->in_paths, this->out);
};
}  // namespace xwim
