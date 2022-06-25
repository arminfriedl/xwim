#pragma once

#include <optional>
#include <set>

#include "util/Common.hpp"
#include "UserOpt.hpp"

namespace xwim {
using namespace std;
using std::filesystem::path;

class UserIntent {
public:
    virtual void execute() = 0;
    virtual ~UserIntent() = default;
};

/* Factory method to construct a UserIntent which implements `execute()` */
unique_ptr<UserIntent> make_intent(const UserOpt& userOpt);

/**
* Extraction intent
*
* Extracts one or multiple archives. Optionally extracts them to given `out` folder. Otherwise extracts them to the
* current working directory.
*/
class ExtractIntent: public UserIntent {
private:
    set<path> archives;
    optional<path> out;

public:
    ExtractIntent(set<path> archives, optional<path> out): archives(archives), out(out) {};
    ~ExtractIntent() = default;

    void execute();
};

/**
* Compress intent for a single file or folder.
*
* Compresses a single path which may be a file or a folder.
*
* No `out` path given:
* - derives the archive name from the input path
* - uses the default archive format for the platform
*
* `out` path given:
* - `out` path must be a path with a valid archive name (including extension)
* - tries to compress the input to the out archive
* - if the `out` base name is different from the input base name, puts the input into a new folder
*   with base name inside the archive (archive base name is always the name of the archive content)
*/
class CompressSingleIntent : public UserIntent {
private:
    path in;
    optional <path> out;

public:
    CompressSingleIntent(path in, optional <path> out) : UserIntent(), in(in), out(out) {};

    ~CompressSingleIntent() = default;

    void execute();
};

/**
 * Compress intent for multiple files and/or folders.
 *
 * Compresses multiple files and/or folders to a single archive as given by the `out` path. Since `out` cannot be
 * guessed from the input in this case it is mandatory.
 *
 * A new, single root folder with base name equal to base name of the `out` archive is created inside the archive. All
 * input files are put into this root folder.
 */
class CompressManyIntent: public UserIntent {
private:
    set<path> in_paths;
    path out;

public:
    CompressManyIntent(set<path> in_paths, path out): UserIntent(), in_paths(in_paths), out(out) {};
    ~CompressManyIntent() = default;

    void execute();
};

}  // namespace xwim
