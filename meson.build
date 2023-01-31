project(
    'volman',
    ['c'],
    version : '1.0',
    license : 'GPL-2.0',
    default_options : [
        'c_std=c99',
    ],
)

c_args = [
    '-Wall',
    '-Wextra',
    '-D_GNU_SOURCE',
]

if get_option('buildtype') == 'plain'
  c_args += '-O2'
endif

app_deps = [
    dependency('gtk+-3.0'),
    dependency('gudev-1.0'),
    dependency('libnotify'),
]

app_src = [
    'context.c',
    'devblock.c',
    'device.c',
    'main.c',
    'printlog.c',
    'tvmnotify.c',
]

executable(
    meson.project_name(),
    c_args : c_args,
    dependencies : app_deps,
    sources : app_src,
    install : true,
)

install_data(
    join_paths('data', meson.project_name() + '.desktop'),
    install_dir: '/etc/xdg/autostart/'
)


