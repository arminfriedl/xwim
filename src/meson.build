xwim_src = ['main.cpp', 'Xwim.cpp', 'Archiver.cpp']
xwim_archiver = ['archiver/LibArchiver.cpp']

xwim_libs = [dependency('libarchive', required: true),
             dependency('fmt', required: true),
             dependency('spdlog', required: true),
             dependency('tclap', required: true)]

executable('xwim', xwim_src+xwim_archiver, dependencies: xwim_libs)
