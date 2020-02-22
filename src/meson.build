src = ['main.cpp',
       'archive.cpp',
       'archive_sys.cpp']
inc = ['archive.hpp',
       'spec.hpp',
       'archive_sys.hpp']

libs = [dependency('libarchive', required: true),
        dependency('fmt', required: true),
        dependency('spdlog', required: true)]

executable('xwim', src, inc, dependencies: libs)
