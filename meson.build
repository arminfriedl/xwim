project('xwim', 'cpp',
        version: '0.4',
        default_options: ['cpp_std=c++17',
                          'warning_level=3',
                          'b_ndebug=if-release'])

add_global_arguments('-DVERSION='+meson.version(), language: 'cpp')

subdir('src')
subdir('doc')
# subdir('test')
