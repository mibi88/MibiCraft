data_sources = [
  'textures.ls',
  'blocks.ls',
]

foreach data : data_sources
  mibicraft_sources += custom_target(
    output: '@BASENAME@.c',
    input: data,
    command: [dataconv, '@INPUT@',
      meson.current_build_dir()+'/@BASENAME@.h',
      '@OUTPUT@'],
    build_by_default: true
  )
endforeach
