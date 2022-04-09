# Run tests+coverage with `ninja clean && ninja test && ninja coverage`
gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')

# subdir('archives')
user_opt_test_exe = executable('user_opt_test_exe',
                              sources: ['user_opt_test.cpp', '../src/UserOpt.cpp'],
                              include_directories: ['../src'],
                              dependencies: [gtest_dep])

test('user opt parsing test', user_opt_test_exe)
