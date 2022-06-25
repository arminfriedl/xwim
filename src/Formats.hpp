#pragma once

namespace xwim {
    using namespace std;

    // Invariant:
    // `extensions_format` defines a surjection from `format_extensions`
    // to `Formats`
    enum class Format {
        UNKNOWN,
        TAR_BZIP2, TAR_GZIP, TAR_LZIP, TAR_XZ, TAR_COMPRESS, TAR_ZSTD,
        ZIP
    };

    const set<string> format_extensions{
            // tar formats see: https://en.wikipedia.org/wiki/Tar_(computing)#Suffixes_for_compressed_files

            /* bzip2 */    ".tar.bz2", ".tb2", ".tbz", ".tbz2", ".tz2",
            /* gzip */     ".tar.gz", ".taz", ".tgz",
            /* lzip */     ".tar.lz",
            /* xz */       ".tar.xz", ".txz",
            /* compress */ ".tar.Z", ".tZ", ".taZ",
            /* zstd */     ".tar.zst", ".tzst",

            /* zip */      ".zip"
    };

    const map<set<string>, Format> extensions_format{
        {{".tar.bz2", ".tb2", ".tbz", ".tbz2", ".tz2"}, Format::TAR_BZIP2},
        {{".tar.gz", ".taz", ".tgz"}, Format::TAR_GZIP},
        {{".tar.lz"}, Format::TAR_LZIP},
        {{".tar.xz", ".txz"}, Format::TAR_XZ},
        {{".tar.Z", ".tZ", ".taZ"}, Format::TAR_COMPRESS},
        {{".tar.zst", ".tzst"}, Format::TAR_ZSTD},

        {{".zip"}, Format::ZIP}
    };

    inline Format find_extension_format(const string& ext) {
        for(auto ef: extensions_format) {
            auto f = ef.first.find(ext);
            if(f != ef.first.end()) {
                return ef.second;
            }
        }

        return Format::UNKNOWN;
    }
}
