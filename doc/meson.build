doxygen = find_program('doxygen', required : false)

if not doxygen.found()
  error('MESON_SKIP_TEST doxygen not found.')
endif

cdata = configuration_data()
cdata.set('VERSION', meson.project_version())

if find_program('dot', required : false).found()
  cdata.set('HAVE_DOT', 'YES')
else
  cdata.set('HAVE_DOT', 'NO')
endif

cdata.set('PROJECT_NAME', meson.project_name())
cdata.set('PROJECT_SRCDIR', join_paths(meson.current_source_dir(),'..','src'))
cdata.set('PROJECT_DOCDIR', meson.current_source_dir())
cdata.set('PROJECT_TESTDIR', join_paths(meson.current_source_dir(),'..','test'))

doxyfile = configure_file(input: 'Doxyfile.in',
                          output: 'Doxyfile',
                          configuration: cdata,
                          install: false)

datadir = join_paths(get_option('datadir'), 'doc','xwim')

# Must be disabled for now until https://github.com/mesonbuild/meson/pull/7458
# lands
# html_target = custom_target('doc',
#                             build_always_stale : not meson.is_subproject(),
#                             input: doxyfile,
#                             output: 'html',
#                             command: [doxygen, '@INPUT@'],
#                             install: not meson.is_subproject(),
#                             install_dir: datadir,
#                             compilers: doxygen)
