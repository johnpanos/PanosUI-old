#ifndef _EGL_H
#define _EGL_H
#endif

#include <wayland-client.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <cairo/cairo.h>
#include <cairo/cairo-gl.h>

class EGLProvider
{
public:
    EGLDisplay *egl_display;
    EGLConfig *egl_config;
    EGLContext *egl_context;

    cairo_device_t *cairo_device;

    void create_window(struct wl_display *display);

    void init_cairo();
};