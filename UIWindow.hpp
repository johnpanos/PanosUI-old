#ifndef _UIWINDOW_H
#define _UIWINDOW_H

#include <wayland-egl.h>
#include <EGL/egl.h>
#include <include/core/SkSurface.h>
#include "WaylandRegistry.hpp"

namespace UI
{
    class Window
    {
    public:
        EGLSurface egl_surface;
        SkSurface *surface;
        wl_egl_window *egl_window;

        WaylandXDG *xdg;
        WaylandXDGSurfaceToplevel *toplevel;
        SkRect frame;

        bool needsRepaint;

        Window(const char *title, SkRect frame);
        ~Window();

        void on_resize(int width, int height);
        void draw();
        void run();
    };
};

#endif