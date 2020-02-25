# Run tests+coverage with `ninja clean && ninja test && ninja coverage`
gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')

xwim_src = ['../src/archive.cpp',
            '../src/archive_sys.cpp']

subdir('archives')

archive_test_exe = executable('archive_test_exe',
                              sources: ['archive_test.cpp', xwim_src],
                              include_directories: ['../src'],
                              dependencies: [gtest_dep, xwim_libs])


test('archive test', archive_test_exe)

fileformats_test_exe = executable('fileformats_test_exe',
                              sources: ['fileformats_test.cpp', xwim_src],
                              include_directories: ['../src'],
                              dependencies: [gtest_dep, xwim_libs])

test('fileformats test', fileformats_test_exe)
