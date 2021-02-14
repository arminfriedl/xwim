xwim_src = ['main.cpp',
            'archive.cpp',
            'archive_sys.cpp',
            'util/argparse.cpp']

xwim_libs = [dependency('libarchive', required: true),
             dependency('fmt', required: true),
             dependency('spdlog', required: true),
             dependency('tclap', required: true)]

executable('xwim', xwim_src, dependencies: xwim_libs)
