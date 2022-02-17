#ifndef _EGL_H
#define _EGL_H
#endif

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

class EGLProvider
{
public:
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

    EGLSurface egl_surface;
    EGLSurface secondarySurface;

    wl_egl_window *egl_window;

    void
    create_window(int32_t width, int32_t height, struct wl_display *display, struct wl_surface *surface);

    void init_cairo();
};