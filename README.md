[![Build Status](https://drone.friedl.net/api/badges/incubator/xwim/status.svg)](https://drone.friedl.net/incubator/xwim)

# XWIM
Do What I Mean Extractor

![https://xkcd.com/1168/](https://imgs.xkcd.com/comics/tar.png)

Continuing the emacs tradition of "Do What I Mean" tools, dtrx is a replacement
for the excellent, but unfortunately unmaintained,
[dtrx](https://github.com/brettcs/dtrx). `xwim` is a command line tool that
targets two problems with archives:

- Command line tools for extracting archives are often archaic and differ
  considerably between formats
- Inconsiderately packaged archives tend to spill their content over the
  directory they are extracted to

# Usage
Invoking `xwim` is as simple as:

```shell
xim archive.tar.gz
```

This will extract the archive `archive.tar.gz` to the current folder. If the
archive contains a single root folder named like the archive, it is just
extracted as is. Otherwise `xwim` first creates a folder named after the archive
and extracts the contents there.

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

`xwim` will just extract the archive to the current directory.

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
`xwim` is indifferent to the actual extraction backend. However, currently only
[libarchive](https://libarchive.org/) is supported. `xwim` supports most formats
supported by `libarchive`:

- 7-zip: 7z, 7zip
- zip: jar, zip 
- bzip2: bz2, bzip2
- gzip: gz, gzip
- xzip: xz
- rar: rar
- tar with compression: tgz, tar.gz, tar.bz2, tar.xz
