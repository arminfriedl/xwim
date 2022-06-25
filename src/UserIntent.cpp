#include "UserIntent.hpp"

#include <algorithm>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "Archiver.hpp"

namespace xwim {

    unique_ptr<UserIntent> make_intent(const UserOpt &userOpt) {
        if (userOpt.wants_compress() && userOpt.wants_extract()) {
            throw XwimError("Cannot compress and extract simultaneously");
        }
        if(userOpt.paths.size() == 0) {
            throw XwimError("No input given...");
        }

        // compression intent explicitly specified
        if (userOpt.wants_compress()) {
            if (userOpt.paths.size() == 1) {
                return make_unique<CompressSingleIntent>(
                        CompressSingleIntent{
                                *userOpt.paths.begin(),
                                userOpt.out
                        });
            }

            if (!userOpt.out.has_value()) {
                throw XwimError("Cannot guess output for multiple targets");
            }

            return make_unique<CompressManyIntent>(
                    CompressManyIntent{
                            userOpt.paths,
                            userOpt.out.value()
                    });
        }

        // extraction intent explicitly specified
        if (userOpt.wants_extract()) {
            for (path p: userOpt.paths) {
                if (!can_handle_archive(p)) {
                    throw XwimError("Cannot extract path {}", p);
                }
            }

            return make_unique<ExtractIntent>(
                    ExtractIntent{
                            userOpt.paths,
                            userOpt.out
                    });
        }

        // no intent explicitly specified, try to infer from input

        bool can_extract_all = std::all_of(
                userOpt.paths.begin(), userOpt.paths.end(),
                [](path path) {
                    return can_handle_archive(path);
                });

        bool is_out_archive = userOpt.out.has_value() && can_handle_archive(userOpt.out.value());

        // out is explicitly specified and an archive, assume we want compression
        if(is_out_archive) {
            if(userOpt.paths.size() == 1) {
                return make_unique<CompressSingleIntent>(
                        CompressSingleIntent{
                           *userOpt.paths.begin(),
                           userOpt.out
                        });
            }

            return make_unique<CompressManyIntent>(
                    CompressManyIntent{
                        userOpt.paths,
                        userOpt.out.value() // this is ok is_out_archive checks for has_value()
                    }
            );
        }

        // all inputs are extractable archives, assume extraction intent
        if (can_extract_all) {
            return make_unique<ExtractIntent>(
                    ExtractIntent{
                            userOpt.paths,
                            userOpt.out
                    });
        }

        // at this point all we can hope for is that the intention is a single-path compression:
        // we don't know how to extract it; we don't know (and can't guess) output for many-path compression;
        if(userOpt.paths.size() == 1) {
            return make_unique<CompressSingleIntent>(
                    CompressSingleIntent{
                        *userOpt.paths.begin(),
                        userOpt.out
                    });
        }

        throw XwimError("Cannot guess intent");
    }


    void ExtractIntent::execute() {
        bool has_out = this->out.has_value();
        bool is_single = this->archives.size() == 1;

        for (path p: this->archives) {
            unique_ptr<Archiver> archiver = make_archiver(p);
            path out;

            if(has_out) {
                if(is_single) { // just dump content of archive into `out`
                    std::filesystem::create_directories(this->out.value());
                    out = this->out.value();
                } else { // create an `out` folder and extract inside there
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

            if(dit == std::filesystem::directory_iterator()) {
                spdlog::debug("Archive is empty");
            } else if(is_directory(dit->path())){
                auto first_path = dit->path();
                auto next_entry = next(dit);

                if(next_entry == std::filesystem::directory_iterator()) {
                    spdlog::debug("Archive has single entry which is a directory");
                    if(std::filesystem::equivalent(first_path.filename(), out.filename())) {
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
        if(this->out.has_value()) {
            if(!can_handle_archive(this->out.value())) {
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
        if(!can_handle_archive(this->out)) {
            throw XwimError("Unknown archive format {}", this->out);
        }

        unique_ptr<Archiver> archiver = make_archiver(this->out);
        archiver->compress(this->in_paths, this->out);
    };
}  // namespace xwim
