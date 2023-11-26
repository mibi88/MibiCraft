mibicraft_sources += [
  'gfx.c',
  'main.c',
  'chunk.c',
  'world.c',
  'entity.c',
]

mibicraft_incdirs = include_directories('.')

mibicraft_deps = [
  dependency('GL', native: true),
  dependency('glu', native: true),
  dependency('freeglut', native: true),
  meson.get_compiler('c').find_library('m', required: true),
]

executable('MibiCraft', mibicraft_sources,
  include_directories: mibicraft_incdirs,
  dependencies: mibicraft_deps,
  install: true,
)

