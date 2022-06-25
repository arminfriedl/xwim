xwim_src = ['main.cpp', 'Archiver.cpp', 'UserOpt.cpp', 'UserIntent.cpp']

xwim_archiver = ['archiver/LibArchiver.cpp']

is_static = get_option('default_library')=='static'

xwim_libs = [dependency('libarchive', required: true, static: is_static),
             dependency('spdlog', required: true, static: is_static),
             dependency('fmt', required: true, static: is_static),
             dependency('tclap', required: true, static: is_static)]

executable('xwim', xwim_src+xwim_archiver, dependencies: xwim_libs)
