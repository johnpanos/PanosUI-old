#ifndef _EGL_H
#define _EGL_H
#endif

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#define SK_GL 1

#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>
#include <include/gpu/GrBackendSurface.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>

#include <include/gpu/gl/GrGLFunctions.h>

class EGLProvider
{
public:
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

    EGLSurface egl_surface;
    EGLSurface secondarySurface;

    wl_egl_window *egl_window;

    int32_t width;
    int32_t height;

    sk_sp<const GrGLInterface> interface;
    GrDirectContext *context;

    sk_sp<SkSurface> surface;

    void
    setup(struct wl_display *display, struct wl_surface *surface, int initial_width, int initial_height);

    void on_resize(int32_t width, int32_t height);

    void create_window(int32_t width, int32_t height, struct wl_display *display, struct wl_surface *surface, int x, int y);

    void draw(int x, int y);
};