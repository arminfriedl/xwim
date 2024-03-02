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

void ExtractIntent::dwim_reparent(const path &out) {
  // move extraction if extraction resulted in only one entry and that entries
  // name is already the stripped archive name, i.e. reduce unnecessary nesting
  auto dit = std::filesystem::directory_iterator(out);
  auto dit_path = dit->path();

  if (dit == std::filesystem::directory_iterator()) {
    spdlog::debug(
        "Cannot flatten extraction folder: extraction folder is empty");
    return;
  }

  if (!is_directory(dit_path)) {
    spdlog::debug("Cannot flatten extraction folder: {} is not a directory",
                  dit_path);
    return;
  }

  if (next(dit) != std::filesystem::directory_iterator()) {
    spdlog::debug("Cannot flatten extraction folder: multiple items extracted");
    return;
  }

  if (!std::filesystem::equivalent(dit_path.filename(), out.filename())) {
    spdlog::debug(
        "Cannot flatten extraction folder: archive entry differs from archive "
        "name [extraction folder: {}, archive entry: {}]",
        out.filename(), dit_path.filename());
    return;
  }

  spdlog::debug("Output folder [{}] is equivalent to archive entry [{}]", out,
                dit_path);
  spdlog::info("Flattening extraction folder");

  int i = rand_int(0, 100000);
  path tmp_out = path{out};
  tmp_out.concat(fmt::format(".xwim{}", i));
  spdlog::debug("Move {} to {}", dit_path, tmp_out);
  std::filesystem::rename(dit_path, tmp_out);
  spdlog::debug("Remove parent path {}", out);
  std::filesystem::remove(out);
  spdlog::debug("Moving {} to {}", tmp_out, out);
  std::filesystem::rename(tmp_out, out);
}

path ExtractIntent::out_path(const path &p) {
  if (!this->out.has_value()) {
    // not out path given, create from archive name
    path out = std::filesystem::current_path() / strip_archive_extension(p);
    create_directories(out);
    return out;
  }

  if (this->archives.size() == 1) {
    // out given and only one archive to extract, just extract into `out`
    create_directories(this->out.value());
    return this->out.value();
  }

  // out given and multiple archives to extract, create subfolder
  // for each archive
  create_directories(this->out.value());
  path out = this->out.value() / strip_archive_extension(p);
  return out;
}

void ExtractIntent::execute() {
  for (const path &p : this->archives) {
    std::unique_ptr<Archiver> archiver = make_archiver(p);
    path out = this->out_path(p);
    archiver->extract(p, out);
    this->dwim_reparent(out);
  }
}

path CompressSingleIntent::out_path() {
  if (this->out.has_value()) {
    if (!can_handle_archive(this->out.value())) {
      throw XwimError("Unknown archive format {}", this->out.value());
    }

    return this->out.value();
  }

  return default_archive(strip_archive_extension(this->in).stem());
}

void CompressSingleIntent::execute() {
  path out = this->out_path();
  unique_ptr<Archiver> archiver = make_archiver(out);
  set<path> ins{this->in};
  archiver->compress(ins, out);
};

void CompressManyIntent::execute() {
  if (!can_handle_archive(this->out)) {
    throw XwimError("Unknown archive format {}", this->out);
  }

  unique_ptr<Archiver> archiver = make_archiver(this->out);
  archiver->compress(this->in_paths, this->out);
}
}  // namespace xwim
