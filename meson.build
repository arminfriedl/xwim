project('xwim', 'cpp',
        version: '0.4',
        default_options: ['cpp_std=c++17',
                          'warning_level=3',
                          'b_ndebug=if-release'])

add_global_arguments('-DVERSION='+meson.version(), language: 'cpp')
add_global_arguments('-DSPDLOG_FMT_EXTERNAL', language: 'cpp')
add_global_arguments('-DFMT_HEADER_ONLY', language: 'cpp')

subdir('src')
subdir('doc')
subdir('test')
