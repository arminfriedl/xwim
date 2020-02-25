xwim_src = ['main.cpp',
            'archive.cpp',
            'archive_sys.cpp']

xwim_inc = ['archive.hpp',
            'spec.hpp',
            'archive_sys.hpp',
            'fileformats.hpp']

xwim_libs = [dependency('libarchive', required: true),
             dependency('fmt', required: true),
             dependency('spdlog', required: true)]

executable('xwim', xwim_src, xwim_inc, dependencies: xwim_libs)
