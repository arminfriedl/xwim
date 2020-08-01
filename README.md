[![Build Status](https://drone.friedl.net/api/badges/incubator/xwim/status.svg)](https://drone.friedl.net/incubator/xwim)

# XWIM
Do What I Mean Extractor

![https://xkcd.com/1168/](https://imgs.xkcd.com/comics/tar.png)

Continuing the emacs tradition of "Do What I Mean" tools, xwim is a replacement
for the excellent, but unfortunately unmaintained,
[dtrx](https://github.com/brettcs/dtrx). xwim is a command line tool that
targets two problems with archives:

- Command line tools for extracting archives are often archaic and differ
considerably between formats
- Inconsiderately packaged archives tend to spill their content over the
directory they are extracted to

# Usage
Invoking `xwim` is as simple as:

```shell
xwim archive.tar.gz
```

This will extract the archive to the current folder. If the archive contains a
single root folder it is just extracted as is. Otherwise xwim first creates a
folder named after the archive and extracts the contents there.

# Examples

## Single root folder named after the archive

```
archive.tar.gz
|
-- archive/
    |
    -- file.txt
    |
    -- file2.txt
```

xwim will just extract the archive to the current directory.

## Multiple files/folders in archive root

```
archive.tar.gz
|
-- archive/
|    |
|    -- file.txt
| 
-- file2.txt
```

`xwim` will create a folder `archive` in the current directory and extract the
archive contents there.

# Supported formats
xwim supports most formats supported by [libarchive](https://libarchive.org/):

- 7-zip: 7z, 7zip
- zip: jar, zip 
- bzip2: bz2, bzip2
- gzip: gz, gzip
- xzip: xz
- rar: rar
- tar with compression: tgz, tar.gz, tar.bz2, tar.xz

# Install
xwim is currently released as a dynamically linked glibc binary only. The
releases can be downloaded from https://git.friedl.net/incubator/xwim/releases
and should run on most glibc based GNU/Linux distributions. The following
dependencies have to be installed:
- [spdlog](https://github.com/gabime/spdlog)
- [fmt](https://github.com/fmtlib/fmt)
- [libarchive](https://github.com/libarchive/libarchive)

Approaching the first stable release we will release for more platforms.

# Build
xwim is built with [meson](https://mesonbuild.com/). To compile xwim from source
you need:
- [meson](https://mesonbuild.com/)
- [ninja](https://ninja-build.org/)
- GCC or Clang (others may work too) supporting C++17

Additionally you need some libraries installed:
- [spdlog](https://github.com/gabime/spdlog)
- [fmt](https://github.com/fmtlib/fmt)
- [libarchive](https://github.com/libarchive/libarchive)


``` shell
# Get the source
git clone https://git.friedl.net/incubator/xwim.git

# Build xwim executable
cd xwim
meson build
cd build
meson compile

# Run executable on the test archive
# This will extract root.tar.gz to 
# the current working directory
src/xwim test/archives/root.tar.gz
```

# Contributing
While xwim is still in incubator phase (i.e. before version 1.0) it's main
repository is hosted on https://git.friedl.net/incubator/xwim with a mirror on
https://github.com/arminfriedl/xwim. With the first stable release it will most
likely move to GitHub as it's main repository.

If you want to contribute, you can either issue a pull request on it's Github
mirror (will be cherry picked into the main repository) or send patches to
dev[at]friedl[dot]net.

If you are interested in a long-term co-maintainership you can also drop me a
mail for an account on https://git.friedl.net.
