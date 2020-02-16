src = ['main.cpp', 'archive.cpp']
inc = ['archive.hpp',
       'spec.hpp']

libs = [dependency('libarchive', required: true),
        dependency('fmt', required: true),
        dependency('spdlog', required: true)]

executable('xwim', src, inc, dependencies: libs)
