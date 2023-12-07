assets_sources = [
  'blocks.png',
  'crosshair.png',
  'font.png',
]

pymod = import('python')

prog_python = pymod.find_installation('python3',
  required: true,
  modules: [
    'sys',
    'PIL',
    'os',
  ],
)

mibicraft_sources = []

foreach image : assets_sources
  mibicraft_sources += custom_target(
    output: '@BASENAME@.c',
    input: image,
    command: [prog_python, img_converter,
      '@INPUT@',
      meson.current_build_dir()+'/@BASENAME@.h',
      '@OUTPUT@'],
    build_by_default: true
  )
endforeach

