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
    'xdg-shell-protocol.c',
    'main.cpp',
    'egl.cpp',

    'wayland/Display.cpp',
    'wayland/Pointer.cpp',
    'wayland/Registry.cpp',
    'wayland/Surface.cpp',
    'wayland/XDGSurface.cpp',
    'wayland/XDGToplevel.cpp',

    'ui/Animation.cpp',
    'ui/Application.cpp',
    'ui/EventResponder.cpp',
    'ui/Layer.cpp',
    'ui/Registry.cpp',
    'ui/Shape.cpp',
    'ui/Skia.cpp',
    'ui/View.cpp',
    'ui/Window.cpp',
    'ui/WindowToplevel.cpp'
]

skia = declare_dependency(link_args : ['-L/home/john/skia/out/linux/x64', '-lskia'],
                               include_directories : ['./thirdparty/skia'])

executable('panos-client', src, dependencies: [wayland_egl, wayland_client, wayland_cursor, wl_roots, egl, glesv2, fontconfig, freetype, glx, skia])