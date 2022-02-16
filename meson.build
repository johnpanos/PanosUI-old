project('PanosUI', ['cpp', 'c'])

wayland_egl = dependency('wayland-egl')
wayland_client = dependency('wayland-client')
wl_roots = dependency('wlroots')
egl = dependency('egl')
glesv2 = dependency('glesv2')

src = [
    'egl.cpp',
    'cairo.cpp',
    'xdg-shell-protocol.c'
]

skia = declare_dependency(link_args : ['-L/home/john/skia/out/Static', '-lskia'],
                               include_directories : ['./thirdparty/skia'])

executable('cairo', src, dependencies: [wayland_egl, wayland_client, wl_roots, egl, glesv2, skia])