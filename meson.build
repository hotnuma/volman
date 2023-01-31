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
]

if get_option('buildtype') == 'plain'
  c_args += '-O2'
endif

# examples
#dependency('dbus-1'),
#dependency('libxml-2.0'),
#dependency('tinyc'),
#dependency('libpcre'),
#dependency('glib-2.0'),
#dependency('gio-2.0'),

app_deps = [
    dependency('gtk+-3.0'),
    dependency('gudev-1.0'),
    dependency('libnotify'),
]

#'tvm-input-device.c',
#'tvm-usb-device.c',
#'tvm-run.c',

app_src = [
    'main.c',
    'tvm-block-device.c',
    'tvm-context.c',
    'tvm-device.c',
    'tvm-gio-extensions.c',
    'tvm-notify.c',
    'tvm-pango-extensions.c',
    'tvm-prompt.c',
]

executable(
    meson.project_name(),
    c_args : c_args,
    dependencies : app_deps,
    sources : app_src,
    install : false,
)


