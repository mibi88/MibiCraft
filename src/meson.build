mibicraft_sources += [
  'client/gfx.c',
  'client/main.c',
  'client/game.c',
  'client/button.c',
  'shared/chunk.c',
  'shared/world.c',
  'shared/entity.c',
  'shared/raycast.c',
  'shared/noise.c',
  'shared/threading.c',
  'shared/player.c',
]

mibicraft_incdirs = include_directories('client', 'shared')

mibicraft_deps = [
  dependency('GL', native: true),
  dependency('glu', native: true),
  dependency('freeglut', native: true),
  meson.get_compiler('c').find_library('m', required: true),
  meson.get_compiler('c').find_library('pthread', required: true),
]

executable('MibiCraft', mibicraft_sources,
  include_directories: mibicraft_incdirs,
  dependencies: mibicraft_deps,
  install: true,
)

