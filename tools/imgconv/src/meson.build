imgconv_sources = [
  'main.c',
  'img.c',
  'conv.c',
]

imgconv_incdirs = include_directories('.')

imgconv_deps = [
  dependency('libpng', native: true),
]

imgconv = executable('imgconv', imgconv_sources,
  include_directories: imgconv_incdirs,
  dependencies: imgconv_deps,
  install: true,
  native: true,
)
