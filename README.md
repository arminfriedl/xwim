[![Build Status](https://drone.friedl.net/api/badges/incubator/xwim/status.svg)](https://drone.friedl.net/incubator/xwim)

# XWIM
Do What I Mean Extractor

![https://xkcd.com/1168/](https://imgs.xkcd.com/comics/tar.png)

[xkcd-1168](https://xkcd.com/1168/)

Continuing the emacs tradition of "Do What I Mean" tools, xwim is replacement
for the excellent, but unfortunately unmaintained,
[dtrx](https://github.com/brettcs/dtrx). xwim is a command line tool that
targets two problems with archives:

- Command line tools for extracting archives are often archaic and differ
considerably between formats
- Inconsiderately packaged archives tend to spill their content over the
directory they are extracted to

`dtrx` is a Python script that sets up the command line and calls appropriate
archiving binaries (if installed). In contrast `xwim` is a compiled binary based
directly on archiving libraries, which some may appreciate. It can optionally be
statically linked if you want it entirely self-contained.

# Usage
Invoking `xwim` is as simple as:

```shell
xwim archive.tar.gz
```

This will extract the archive to the current folder. If the archive contains a
single root folder it is just extracted as is. Otherwise xwim creates a folder
named after the archive and extracts the contents there.


```shell
xwim /home/user/
```

This will create an archive in the "platform native" format (zip on windows,
tar.gz on unix) in the current working directory. The archive contains a single
root folder `user` and is itself named `user.zip` or `user.tar.gz`.

```shell
xwim /home/user/file.txt
```

This will create an archive in the "platform native" format (zip on windows,
tar.gz on unix) in the current working directory. The archive contains a single
entry `file.txt` and is itself named `file.zip` or `file.tar.gz`.

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

xwim will create a folder `archive` in the current directory and extract the
archive contents there.

# Supported formats
Currently `xwim` supports `tar.gz` and `zip` archives. However, this will
rapidly expand to many more formats until a stable release is officially
announced.

Take a look `Archiver.hpp` if you want to help and have some time for testing.
Most formats can readily be added if they are supported by libarchive. For other
formats you have to add an `Archiver` implementation.

# Install
`xwim` currently released for Linux only. There are two flavers: statically
linked and dynamically linked. The releases can be downloaded from
https://git.friedl.net/incubator/xwim/releases and should run on most 64-bit
GNU/Linux distributions. 

For the dynamically linked version, the following dependencies have to be
installed:
- [spdlog](https://github.com/gabime/spdlog)
- [fmt](https://github.com/fmtlib/fmt)
- [libarchive](https://github.com/libarchive/libarchive)

Windows support is planned for the first stable release. Packaging for various
distributions is also planned once `xwim` stabilizes. Please reach out if you
can help.

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

# Configure
xwim strives to just do the right thing out of the box. Consequently, it does
not require any configuration. If you are unhappy with the defaults you can
change them though.

## Changing the log level
Per default xwim chooses an appropriate log level according to your build type
(debug/release builds). If you want to change the verbosity you can set the
`XWIM_LOGLEVEL` environment variable. Valid levels are:
- trace
- debug
- info
- warning
- error
- critical
- off

# Contributing
While xwim is still in incubator phase (i.e. before version 1.0) its main
repository is hosted on https://git.friedl.net/incubator/xwim with a mirror on
https://github.com/arminfriedl/xwim. With the first stable release it will most
likely move to GitHub as its main repository.

If you want to contribute, you can either issue a pull request on its Github
mirror (will be cherry picked into the main repository) or send patches to
dev[at]friedl[dot]net.

If you are interested in a long-term co-maintainership you can also drop me a
mail for an account on https://git.friedl.net.

# Known Issues

- <strong>Parsing filters is unsupported</strong> 
  There is a somewhat long standing
  [bug](https://github.com/libarchive/libarchive/issues/373) in libarchive. rar
  files might fail with `Parsing filters is unsupported`. This is because `rar`
  is a proprietary format and `libarchive` does not implement the full machinery
  necessary to support `rar` completely. `xwim` is all about convenience. If you
  want to help with supporting `rar`, please keep in mind that this means we
  have we want to take the [official `unrar`
  library](https://www.rarlab.com/rar_add.htm) if possible. This is also a
  licensing issue as `unrar` is proprietary and its license seemingly not GPL
  compatible.
