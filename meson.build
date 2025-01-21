project('MibiCraft', 'c',
          version: 'v.0.1',
    meson_version: '>= 0.60.0',
  default_options: [ 'warning_level=3',
                     'c_std=c89',
                   ],
)

config_h = configuration_data()
r = run_command('git', 'rev-parse', 'HEAD', check: true)
commit = r.stdout().strip()
config_h.set_quoted('VERSION', meson.project_version()+'-git'+commit)
config_h.set_quoted('NAME', meson.project_name())
configure_file(
  output: 'buildconfig.h',
  configuration: config_h,
)
add_project_arguments([
  '-I' + meson.project_build_root(),
  '-I' + meson.project_build_root() + '/img',
], language: 'c')

subdir('tools')
subdir('img')
subdir('data')

subdir('src')
