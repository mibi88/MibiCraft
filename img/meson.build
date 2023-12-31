assets_sources = [
  'blocks.png',
  'crosshair.png',
  'font.png',
]

mibicraft_sources = []

foreach image : assets_sources
  mibicraft_sources += custom_target(
    output: '@BASENAME@.c',
    input: image,
    command: [imgconv, '@INPUT@',
      meson.current_build_dir()+'/@BASENAME@.h',
      '@OUTPUT@'],
    build_by_default: true
  )
endforeach
