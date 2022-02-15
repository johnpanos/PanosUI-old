#ifndef _EGL_H
#define _EGL_H
#endif

#include <wayland-client.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

class EGLProvider
{
public:
    EGLDisplay *egl_display;
    EGLConfig *egl_config;
    EGLContext *egl_context;

    void create_window(struct wl_display *display);
};