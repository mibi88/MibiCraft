dataconv_sources = [
  'main.c',
  'arena.c',
  'tree.c',
  'parser.c',
]

dataconv_incdirs = include_directories('.')

dataconv_deps = [
]

dataconv = executable('dataconv', dataconv_sources,
  include_directories: dataconv_incdirs,
  dependencies: dataconv_deps,
  install: true,
  native: true,
)

