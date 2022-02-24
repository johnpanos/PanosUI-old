project('PanosUI', ['cpp', 'c'], default_options: ['cpp_std=c++17'])

wayland_egl = dependency('wayland-egl')
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')
wl_roots = dependency('wlroots')
egl = dependency('egl')
glesv2 = dependency('glesv2')
fontconfig = dependency('fontconfig')
freetype = dependency('freetype2')
glx = dependency('glx')

src = [
    'main.cpp',
    'UIApplication.cpp',
    'UIShape.cpp',
    'UIAnimation.cpp',
    'UIWindow.cpp',
    'UIEventResponder.cpp',
    'UILayer.cpp',
    'UIView.cpp',
    'WaylandRegistry.cpp',
    'WaylandPointer.cpp',
    'egl.cpp',
    'xdg-shell-protocol.c',
    'wlr-layer-shell-unstable-v1.c'
]

skia = declare_dependency(link_args : ['-L/home/john/skia/out/linux/x64', '-lskia'],
                               include_directories : ['./thirdparty/skia'])

executable('panos-client', src, dependencies: [wayland_egl, wayland_client, wayland_cursor, wl_roots, egl, glesv2, fontconfig, freetype, glx, skia])