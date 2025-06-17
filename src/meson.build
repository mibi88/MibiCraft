mibicraft_sources += [
  'client/main.c',
  'client/game.c',
  'client/button.c',
  
  'client/gfx/base.c',
  'client/gfx/config.c',
  'client/gfx/image.c',
  'client/gfx/model.c',
  'client/gfx/shapes.c',
  'client/gfx/window.c',
  
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
  meson.get_compiler('c').find_library('X11', required: true),
  meson.get_compiler('c').find_library('Xfixes', required: false),
  meson.get_compiler('c').find_library('m', required: true),
  meson.get_compiler('c').find_library('pthread', required: true),
]

has_xfixes = meson.get_compiler('c').find_library('Xfixes', required: false)

executable('MibiCraft', mibicraft_sources,
  include_directories: mibicraft_incdirs,
  dependencies: mibicraft_deps,
  install: true,
  c_args: [
    has_xfixes.found() ? '-DHAS_XFIXES' : '',
  ],
)

